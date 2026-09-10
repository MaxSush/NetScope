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
			INSERT INTO processes
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

		if (process.end_time.has_value())
		{
			result = statement->Bind(3, process.end_time.value());
		}
		else
		{
			result = statement->BindNull(3);
		}
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
			FROM processes
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
		if (!statement->IsNull(2))
		{
			process.end_time = statement->GetInt64(2);
		}
		process.name = statement->GetText(3);
		process.exe_path = statement->GetText(4);

		return process;
	}

	Expected<int64_t, DatabaseResult> ProcessRepository::GetProcessId(uint32_t pid, uint64_t startTime)
	{
		constexpr auto Sql = R"sql(
			SELECT process_id FROM processes
			WHERE pid = ? AND start_time = ?;
		)sql";

		std::unique_ptr<Statement> statement;
		auto result = m_db.Prepare(Sql, statement);
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
		if (step == StepResult::ROW)
		{
			return static_cast<int64_t>(statement->GetInt64(0));
		}
		else if (step == StepResult::DONE)
		{
			return DatabaseResult::Failure(
				DatabaseStatus::NOT_FOUND, DatabaseError::NONE,
				fmt::format("Process not found: pid{}, start_time = {}", pid, startTime)
			);
		}
		return DatabaseResult::Failure(
			DatabaseStatus::FAILED, DatabaseError::STEP_FAILED,
			"Failed to get process_id"
		);
	}

}
