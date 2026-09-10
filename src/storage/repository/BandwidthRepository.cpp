#include "BandwidthRepository.h"


namespace netscope::storage
{
    netscope::storage::BandwidthRepository::BandwidthRepository(IDatabase& database)
        : m_db(database)
    {
    }

    DatabaseResult BandwidthRepository::AccumulateHourly(int64_t processId, int64_t hourTimestamp, uint64_t uploadBytes, uint64_t downloadBytes, uint32_t flowCount)
    {
        constexpr auto Sql = R"sql(
            INSERT INTO bandwidth_hourly
                (process_id, hour_timestamp, upload_bytes, download_bytes, flow_count)
            VALUES (?, ?, ?, ?, ?)
            ON CONFLICT(process_id, hour_timestamp)
            DO UPDATE SET
                upload_bytes = upload_bytes + excluded.upload_bytes,
                download_bytes = download_bytes + excluded.download_bytes,
                flow_count = MAX(flow_count, excluded.flow_count);
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(1, processId);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(2, hourTimestamp);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(3, uploadBytes);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(4, downloadBytes);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(5, flowCount);
        if (!result)
        {
            return result;
        }

        if (statement->Execute() != StepResult::DONE)
        {
            return DatabaseResult::Failure(
                DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED,
                "Failed to accumulate hourly Bandwidth");
        }
        return DatabaseResult::Ok();
    }

    DatabaseResult BandwidthRepository::RollUpDaily(int64_t dayTimestamp)
    {
        constexpr auto Sql = R"sql(
            INSERT INTO bandwidth_daily
                (process_id, day_timestamp, upload_bytes, download_bytes, flow_count)
            SELECT
                process_id,
                ?,
                SUM(upload_bytes),
                SUM(download_bytes),
                SUM(flow_count)
            FROM bandwidth_hourly
            WHERE hour_timestamp >= ? AND hour_timestamp < ? + 86400000
            GROUP BY process_id
            ON CONFLICT(process_id, day_timestamp)
            DO UPDATE SET
                upload_bytes = excluded.upload_bytes,
                download_bytes = excluded.download_bytes,
                flow_count = excluded.flow_count;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(1, dayTimestamp);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(2, dayTimestamp);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(3, dayTimestamp);
        if (!result)
        {
            return result;
        }

        if (statement->Execute() != StepResult::DONE)
        {
            return DatabaseResult::Failure(
                DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED,
                "Failed to roll up daily bandwidth"
            );
        }
        return DatabaseResult::Ok();
    }

    DatabaseResult BandwidthRepository::PurgeHourlyBefore(int64_t cutoffTimestamp)
    {
        constexpr auto Sql = R"sql(
            DELETE FROM bandwidth_hourly WHERE hour_timestamp < ?;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(1, cutoffTimestamp);
        if (!result)
        {
            return result;
        }

        statement->Execute();
        return DatabaseResult::Ok();
    }

    DatabaseResult BandwidthRepository::PurgeDailyBefore(int64_t cutoffTimestamp)
    {
        constexpr auto Sql = R"sql(
            DELETE FROM bandwidth_daily WHERE day_timestamp < ?;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(1, cutoffTimestamp);
        if (!result) 
        {
            return result;
        }

        statement->Execute();
        return DatabaseResult::Ok();
    }
}