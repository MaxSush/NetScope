#include "FlowRepository.h"


namespace netscope::storage
{

	FlowRepository::FlowRepository(IDatabase& database)
		:m_db(database)
	{
	}

	DatabaseResult FlowRepository::Upsert(const flow::Flow& flow)
	{
		constexpr auto UpsertSql = R"sql(
            INSERT INTO Flows
            (
                process_id,
                protocol,
                local_address,
                local_port,
                remote_address,
                remote_port,
                upload_bytes,
                download_bytes,
                upload_packets,
                download_packets,
                first_seen,
                last_seen,
                state
            )
            VALUES
            (
                ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
            )
            ON CONFLICT
            (
                process_id,
                protocol,
                local_address,
                local_port,
                remote_address,
                remote_port
            )
            DO UPDATE SET
                upload_bytes = excluded.upload_bytes,
                download_bytes = excluded.download_bytes,
                upload_packets = excluded.upload_packets,
                download_packets = excluded.download_packets,
                last_seen = excluded.last_seen,
                state = excluded.state;
		)sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(UpsertSql, statement);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(1, static_cast<uint32_t>(flow.GetPID()));
        if (!result)
            return result;

        auto& key = flow.GetFlowKey();
        result = statement->Bind(2, static_cast<uint32_t>(key.protocol));
        if (!result)
            return result;
        
        result = statement->Bind(3, key.source.ToString());
        if (!result)
            return result;

        result = statement->Bind(4, static_cast<uint32_t>(key.source.port));
        if (!result)
            return result;

        result = statement->Bind(5, key.destination.ToString());
        if (!result)
            return result;

        result = statement->Bind(6, static_cast<uint32_t>(key.destination.port));
        if (!result)
            return result;

        auto& stat = flow.GetStatistics();
        result = statement->Bind(7, stat.uploadBytes);
        if (!result)
            return result;

        result = statement->Bind(8, stat.downloadBytes);
        if (!result)
            return result;

        result = statement->Bind(9, stat.uploadPackets);
        if (!result)
            return result;

        result = statement->Bind(10, stat.downloadPackets);
        if (!result)
            return result;

        result = statement->Bind(11, ToUnixMilliseconds(flow.m_firstSeen));
        if (!result)
            return result;

        result = statement->Bind(12, ToUnixMilliseconds(flow.m_lastSeen));
        if (!result)
            return result;

        // TODO: Save state of  flow
        //result = statement->Bind(13, );
        //if (!result)
        //    return result;

        if (statement->Execute() != StepResult::DONE)
        {
            return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED, "Sql Step execution Failed.");
        }
		return DatabaseResult::Ok();
	}
}