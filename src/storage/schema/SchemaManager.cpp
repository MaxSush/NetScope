#include "SchemaManager.h"
#include <database/IDatabase.h>

namespace netscope::storage
{
	DatabaseResult netscope::storage::SchemaManager::Initialize(IDatabase& db)
	{
		// Enable WAL mode + foreign keys
		auto result = db.Execute("PRAGMA journal_mode = WAL;");
		if (!result)
		{
			return result;
		}

		result = db.Execute("PRAGMA foreign_keys = ON;");
		if (!result)
		{
			return result;
		}

		result = db.Execute("PRAGMA busy_timeout = 5000;");
		if (!result)
		{
			return result;
		}

		result = CreateTables(db);
		if (!result)
		{
			return result;
		}

		result = CreateIndexes(db);
		if (!result)
		{
			return result;
		}

		result = SetSchemaVersion(db);
		if (!result)
		{
			return result;
		}

		return DatabaseResult::Ok();
	}

	DatabaseResult SchemaManager::CreateTables(IDatabase& db)
	{
		// ── schema_version ──
		constexpr auto SchemaVersionSql = R"sql(
			CREATE TABLE IF NOT EXISTS schema_version (
                version     INTEGER PRIMARY KEY,
                description TEXT NOT NULL,
                applied_at  INTEGER NOT NULL
            );
		)sql";

		auto result = db.Execute(SchemaVersionSql);
		if (!result)
		{
			return result;
		}

		// ── app_state ──
		constexpr auto AppStateSql = R"sql(
            CREATE TABLE IF NOT EXISTS app_state (
                key   TEXT PRIMARY KEY,
                value TEXT NOT NULL
            );
        )sql";

		result = db.Execute(AppStateSql);
		if (!result)
		{
			return result;
		}

		// ── processes ──
		constexpr auto ProcessesSql = R"sql(
			CREATE TABLE IF NOT EXISTS processes (
                process_id  INTEGER PRIMARY KEY AUTOINCREMENT,
                pid         INTEGER NOT NULL,
                start_time  INTEGER NOT NULL,
                end_time    INTEGER,
                name        TEXT NOT NULL,
                exe_path    TEXT NOT NULL DEFAULT '',

                UNIQUE(pid, start_time)
            );
		)sql";

		result = db.Execute(ProcessesSql);
		if (!result)
		{
			return result;
		}

		// ── flows (with CHECK constraints as enums) ──
		constexpr auto FlowsSql = R"sql(
            CREATE TABLE IF NOT EXISTS flows (
                flow_id          INTEGER PRIMARY KEY AUTOINCREMENT,
                process_id       INTEGER NOT NULL,

                protocol         INTEGER NOT NULL
                    CHECK(protocol IN (6, 17)),

                local_address    TEXT NOT NULL,
                local_port       INTEGER NOT NULL,
                remote_address   TEXT NOT NULL,
                remote_port      INTEGER NOT NULL,

                upload_bytes     INTEGER NOT NULL DEFAULT 0,
                download_bytes   INTEGER NOT NULL DEFAULT 0,
                upload_packets   INTEGER NOT NULL DEFAULT 0,
                download_packets INTEGER NOT NULL DEFAULT 0,

                first_seen       INTEGER NOT NULL,
                last_seen        INTEGER NOT NULL,

                state            TEXT NOT NULL DEFAULT 'ACTIVE'
                    CHECK(state IN ('ACTIVE', 'IDLE', 'CLOSED')),

                close_reason     TEXT NOT NULL DEFAULT 'NOT_CLOSED'
                    CHECK(close_reason IN (
                        'NOT_CLOSED', 'TCP_FIN', 'TCP_RESET',
                        'CONNECTION_REFUSED', 'TCP_TIMEOUT',
                        'PROCESS_EXITED', 'IDLE_TIMEOUT',
                        'KERNEL_ERROR', 'CAPTURE_STOPPED', 'UNKNOWN'
                    )),

                FOREIGN KEY(process_id) REFERENCES processes(process_id),
                UNIQUE(process_id, protocol, local_address, local_port,
                       remote_address, remote_port)
            );
		)sql";

		result = db.Execute(FlowsSql);
		if (!result)
		{
			return result;
		}

		// ── bandwidth_hourly ──
		constexpr auto BandwidthHourlySql = R"sql(
            CREATE TABLE IF NOT EXISTS bandwidth_hourly (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                process_id      INTEGER NOT NULL,
                hour_timestamp  INTEGER NOT NULL,

                upload_bytes    INTEGER NOT NULL DEFAULT 0,
                download_bytes  INTEGER NOT NULL DEFAULT 0,
                flow_count      INTEGER NOT NULL DEFAULT 0,

                FOREIGN KEY(process_id) REFERENCES processes(process_id),
                UNIQUE(process_id, hour_timestamp)
            );
        )sql";

		result = db.Execute(BandwidthHourlySql);
		if (!result) 
		{
			return result;
		}

		// ── bandwidth_daily ──
		constexpr auto BandwidthDailySql = R"sql(
            CREATE TABLE IF NOT EXISTS bandwidth_daily (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                process_id      INTEGER NOT NULL,
                day_timestamp   INTEGER NOT NULL,

                upload_bytes    INTEGER NOT NULL DEFAULT 0,
                download_bytes  INTEGER NOT NULL DEFAULT 0,
                flow_count      INTEGER NOT NULL DEFAULT 0,

                FOREIGN KEY(process_id) REFERENCES processes(process_id),
                UNIQUE(process_id, day_timestamp)
            );
        )sql";

		result = db.Execute(BandwidthDailySql);
		if (!result) return result;

		return DatabaseResult::Ok();
	}

	DatabaseResult SchemaManager::CreateIndexes(IDatabase& db)
	{
		constexpr auto IndexesSql = R"sql(
			CREATE INDEX IF NOT EXISTS idx_processes_pid
                ON processes(pid);

            CREATE INDEX IF NOT EXISTS idx_flows_active
                ON flows(state) WHERE state != 'CLOSED';

            CREATE INDEX IF NOT EXISTS idx_flows_process_id
                ON flows(process_id);

            CREATE INDEX IF NOT EXISTS idx_flows_last_seen
                ON flows(last_seen);

            CREATE INDEX IF NOT EXISTS idx_flows_closed_time
                ON flows(last_seen) WHERE state = 'CLOSED';

            CREATE INDEX IF NOT EXISTS idx_bandwidth_hourly_time
                ON bandwidth_hourly(hour_timestamp);

            CREATE INDEX IF NOT EXISTS idx_bandwidth_daily_time
                ON bandwidth_daily(day_timestamp);
		)sql";

		return db.Execute(IndexesSql);
	}

	DatabaseResult SchemaManager::SetSchemaVersion(IDatabase& db)
	{
		constexpr auto InsertSql = R"sql(
            INSERT OR IGNORE INTO schema_version (version, description, applied_at)
            VALUES (1, 'Initial schema with process_id FK, CHECK constraints, bandwidth tables', strftime('%s','now') * 1000);
        )sql";
		return db.Execute(InsertSql);
	}
}