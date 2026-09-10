#include "AppStateRepository.h"


namespace netscope::storage
{
	netscope::storage::AppStateRepository::AppStateRepository(IDatabase& database)
		: m_db(database)
	{
	}

	DatabaseResult AppStateRepository::Set(const std::string& key, const std::string& value)
	{
		constexpr auto Sql = R"sql(
			INSERT INTO app_state (key, value) VALUES (?, ?)
            ON CONFLICT(key) DO UPDATE SET value = excluded.value;
		)sql";

		std::unique_ptr<Statement> statement;
		auto result = m_db.Prepare(Sql, statement);
		if (!result)
		{
			return result;
		}

		result = statement->Bind(1, key);
		if (!result)
		{
			return result;
		}

		result = statement->Bind(2, value);
		if (!result)
		{
			return result;
		}

		if (statement->Execute() != StepResult::DONE)
		{
			return DatabaseResult::Failure(
				DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED,
				"Failed to set app_state: " + key);
		}
		return DatabaseResult::Ok();
	}

	std::optional<std::string> AppStateRepository::Get(const std::string& key)
	{
		constexpr auto Sql = R"sql(
			SELECT value FROM app_state WHERE key = ?;
		)sql";

		std::unique_ptr<Statement> statement;
		auto result = m_db.Prepare(Sql, statement);
		if (!result)
		{
			return std::nullopt;
		}

		result = statement->Bind(1, key);
		if (!result)
		{
			return std::nullopt;
		}

		if (statement->Execute() == StepResult::ROW)
		{
			return statement->GetText(0);
		}
		return std::nullopt;
	}
}