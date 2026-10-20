#include "FlowRepository.h"

namespace netscope::storage
{

	FlowRepository::FlowRepository(IDatabase& database)
		:m_db(database)
	{
	}

	DatabaseResult FlowRepository::Upsert(const flow::FlowSnapshot& flowSnapshot, int64_t processId, const std::optional<std::string>& hostname, const std::optional<std::string>& interfaceName)
	{
		constexpr auto UpsertSql = R"sql(
            INSERT INTO flows
            (
                process_id,
                protocol,
                local_address,
                local_port,
                remote_address,
                remote_port,
                remote_hostname,
                upload_bytes,
                download_bytes,
                upload_packets,
                download_packets,
                first_seen,
                last_seen,
                state,
                close_reason,
                interface_name
            )
            VALUES
            (
                ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
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
                remote_hostname = COALESCE(excluded.remote_hostname, remote_hostname),
                upload_bytes = excluded.upload_bytes,
                download_bytes = excluded.download_bytes,
                upload_packets = excluded.upload_packets,
                download_packets = excluded.download_packets,
                last_seen = excluded.last_seen,
                state = excluded.state,
                close_reason = excluded.close_reason,
                interface_name = COALESCE(excluded.interface_name, interface_name);
		)sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(UpsertSql, statement);
        if (!result)
        {
            return result;
        }

        result = statement->Bind(1, static_cast<int64_t>(processId));
        if (!result)
            return result;

        result = statement->Bind(2, static_cast<uint32_t>(flowSnapshot.protocol));
        if (!result)
            return result;
        
        result = statement->Bind(3, flowSnapshot.source.address.ToString());
        if (!result)
            return result;

        result = statement->Bind(4, static_cast<uint32_t>(flowSnapshot.source.port));
        if (!result)
            return result;

        result = statement->Bind(5, flowSnapshot.destination.address.ToString());
        if (!result)
            return result;

        result = statement->Bind(6, static_cast<uint32_t>(flowSnapshot.destination.port));
        if (!result)
            return result;

        if (hostname.has_value() && !hostname->empty())
        {
            result = statement->Bind(7, *hostname);
        }
        else
        {
            result = statement->BindNull(7);
        }
        if (!result)
            return result;

        result = statement->Bind(8, flowSnapshot.statistics.uploadBytes);
        if (!result)
            return result;

        result = statement->Bind(9, flowSnapshot.statistics.downloadBytes);
        if (!result)
            return result;

        result = statement->Bind(10, flowSnapshot.statistics.uploadPackets);
        if (!result)
            return result;

        result = statement->Bind(11, flowSnapshot.statistics.downloadPackets);
        if (!result)
            return result;

        result = statement->Bind(12, ToUnixMilliseconds(flowSnapshot.firstSeen));
        if (!result)
            return result;

        result = statement->Bind(13, ToUnixMilliseconds(flowSnapshot.lastSeen));
        if (!result)
            return result;

        result = statement->Bind(14, flow::ToFlowStateString(flowSnapshot.state));
        if (!result)
            return result;

        result = statement->Bind(15, ToCloseReasonString(flowSnapshot.terminationInfo.closeReason));
        if (!result)
            return result;

        if (interfaceName.has_value() && !interfaceName->empty())
        {
            result = statement->Bind(16, *interfaceName);
        }
        else
        {
            result = statement->BindNull(16);
        }
        if (!result) 
            return result;

        if (statement->Execute() != StepResult::DONE)
        {
            return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED, "FlowRepository::Upsert -> Sql Step execution Failed.");
        }
		return DatabaseResult::Ok();
	}
}