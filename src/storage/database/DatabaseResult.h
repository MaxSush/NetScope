#pragma once

#include <string>

namespace netscope::storage
{
	enum class DatabaseError
	{
		NONE,
		NOT_OPEN,
		OPEN_FAILED,
		CLOSE_FAILED,
		EXECUTE_FAILED,
		PREPARE_FAILED,
		BIND_FAILED,
		STEP_FAILED,
		TRANSACTION_FAILED,
		UNKNOWN
	};

	enum class DatabaseStatus
	{
		SUCCESS,
		NOT_FOUND,
		FAILED
	};

	struct DatabaseResult
	{
		DatabaseStatus status = DatabaseStatus::SUCCESS;
		DatabaseError code = DatabaseError::NONE;
		std::string message;

		[[nodiscard]]
		bool Success() const
		{
			return status == DatabaseStatus::SUCCESS;
		}

		[[nodiscard]]
		bool Found() const
		{
			return status != DatabaseStatus::NOT_FOUND;
		}

		explicit operator bool() const
		{
			return Success();
		}

		static DatabaseResult Ok()
		{
			return {};
		}

		static DatabaseResult Failure(DatabaseStatus status, DatabaseError error, std::string msg)
		{
			return { status, error, std::move(msg) };
		}
	};
}