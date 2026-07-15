#include "ProcessRepository.h"
#include <memory>
#include <database/Statement.h>
#include <fmt/core.h>

namespace netscope::storage
{
	ProcessRepository::ProcessRepository(IDatabase& database)
		: m_db(database)
	{
	}

	DatabaseResult ProcessRepository::Upsert(const process::ProcessInfo& process)
	{
		constexpr auto UpsertSql = R"sql(
			INSERT INTO Processes
			(
				pid,
				start_time,
				end_time,
				name,
				exe_path
			)
			VALUES
			(
				?, ?, ?, ?, ?
			)
			ON CONFLICT(pid, start_time)
			DO UPDATE SET
				end_time = excluded.end_time,
				name = excluded.name,
				exe_path = excluded.exe_path;
		)sql";

		std::unique_ptr<Statement> statement;
		auto result = m_db.Prepare(UpsertSql, statement);
		if (!result)
		{
			return result;
		}

		// Binding
		result = statement->Bind(1, process.pid);
		if(!result)
			return result;

		result = statement->Bind(2, process.start_time);
		if(!result)
			return result;

		result = statement->BindNull(3);
		if(!result)
			return result;

		result = statement->Bind(4, process.name);
		if(!result)
			return result;

		result = statement->Bind(5, process.exe_path);
		if (!result)
			return result;

		if (statement->Execute() != StepResult::DONE)
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::STEP_FAILED, "Sql Step execution Failed.");
		}
		return DatabaseResult::Ok();
	}

	Expected<process::ProcessInfo, DatabaseResult> ProcessRepository::Find(uint32_t pid, uint64_t startTime)
	{
		constexpr auto FindSql = R"sql(
			SELECT
				pid,
				start_time,
				end_time,
				name,
				exe_path
			FROM Processes
			WHERE pid = ?
			AND start_time = ?;
		)sql";

		std::unique_ptr<Statement> statement;

		auto result = m_db.Prepare(FindSql, statement);
		if (!result)
		{
			return result;
		}

		result = statement->Bind(1, pid);
		if (!result)
		{
			return result;
		}
		result = statement->Bind(2, startTime);
		if (!result)
		{
			return result;
		}

		auto step = statement->Execute();
		if (step == StepResult::DONE)
		{
			return DatabaseResult::Failure(DatabaseStatus::NOT_FOUND, DatabaseError::NONE, fmt::format("Row Not Found with pid: {}, start_time: {}", pid, startTime));
		}
		else if (step == StepResult::ERROR)
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::STEP_FAILED, "Sql Step execution Failed.");
		}

		process::ProcessInfo process{};
		process.pid = statement->GetInt32(0);
		process.start_time = statement->GetInt64(1);
		// TODO: end_time
		process.name = statement->GetText(3);
		process.exe_path = statement->GetText(4);

		return process;
	}

}
