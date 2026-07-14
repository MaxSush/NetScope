#include "SchemaManager.h"
#include <database/IDatabase.h>

namespace netscope::storage
{
	DatabaseResult netscope::storage::SchemaManager::Initialize(IDatabase& db)
	{
		auto result = CreateTables(db);
		if (!result)
		{
			return result;
		}

		result = CreateIndexes(db);
		if (!result)
		{
			return result;
		}

		return DatabaseResult::Ok();
	}

	DatabaseResult SchemaManager::CreateTables(IDatabase& db)
	{
		static constexpr auto ProcessTableSql = R"sql(
			CREATE TABLE IF NOT EXISTS Processes (
				process_id INTEGER PRIMARY KEY AUTOINCREMENT,
				
				pid INTEGER NOT NULL,

				start_time INTEGER NOT NULL,

				end_time INTEGER,

				name TEXT NOT NULL,

				exe_path TEXT NOT NULL,
				
				UNIQUE(pid, start_time)
			);
		)sql";

		auto result = db.Execute(ProcessTableSql);
		if (!result)
		{
			return result;
		}

		constexpr auto FlowsTableSql = R"sql(
			CREATE TABLE IF NOT EXISTS Flows(
				flow_id INTEGER PRIMARY KEY AUTOINCREMENT,

				process_id INTEGER NOT NULL,

				protocol INTEGER NOT NULL,

				local_address TEXT NOT NULL,

				local_port INTEGER NOT NULL,

				remote_address TEXT NOT NULL,

				remote_port INTEGER NOT NULL,

				upload_bytes INTEGER NOT NULL DEFAULT 0,

				download_bytes INTEGER NOT NULL DEFAULT 0,

				upload_packets INTEGER NOT NULL DEFAULT 0,

				download_packets INTEGER NOT NULL DEFAULT 0,

				first_seen INTEGER NOT NULL,

				last_seen INTEGER NOT NULL,

				state INTEGER NOT NULL,

				FOREIGN KEY(process_id)
					REFERENCES Processes(process_id)
				UNIQUE(
					process_id,
					protocol,
					local_address,
					local_port,
					remote_address,
					remote_port
				)
			);
		)sql";

		result = db.Execute(FlowsTableSql);
		if (!result)
		{
			return result;
		}

		return DatabaseResult::Ok();
	}

	DatabaseResult SchemaManager::CreateIndexes(IDatabase& db)
	{
		constexpr auto index = R"sql(
			CREATE INDEX IF NOT EXISTS idx_process_pid
			ON Processes(pid);

			CREATE INDEX IF NOT EXISTS idx_flow_process
			ON Flows(process_id);

			CREATE INDEX IF NOT EXISTS idx_flow_last_seen
			ON Flows(last_seen);
		)sql";

		return db.Execute(index);
	}
}