#include "Statement.h"

#include <sqlite3.h>

namespace netscope::storage
{
	Statement::Statement(sqlite3_stmt* statement)
		: m_stmt(statement)
	{
	}

	Statement::~Statement()
	{
		if (m_stmt != nullptr)
		{
			sqlite3_finalize(m_stmt);
		}
	}

	Statement::Statement(Statement&& other) noexcept
		: m_stmt(other.m_stmt)
	{
		other.m_stmt = nullptr;
	}

	Statement& Statement::operator=(Statement&& other) noexcept
	{
		if (this != &other)
		{
			if (m_stmt)
			{
				sqlite3_finalize(m_stmt);
			}

			m_stmt = other.m_stmt;
			other.m_stmt = nullptr;
		}

		return *this;
	}

	DatabaseResult Statement::Bind(int index, uint32_t val)
	{
		if (sqlite3_bind_int(m_stmt, index, val) != SQLITE_OK)
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::BIND_FAILED, "Failed to bind Integer[32].");
		}
		return DatabaseResult::Ok();
	}

	DatabaseResult Statement::Bind(int index, uint64_t val)
	{
		if (sqlite3_bind_int64(m_stmt, index, val) != SQLITE_OK) 
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::BIND_FAILED, "Failed to bind Integer[64].");
		}
		return DatabaseResult::Ok();
	}

	DatabaseResult Statement::Bind(int index, const std::string& val)
	{
		if (sqlite3_bind_text(m_stmt, index, val.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::BIND_FAILED, "Failed to bind Text");
		}
		return DatabaseResult::Ok();
	}

	DatabaseResult Statement::BindNull(int index)
	{
		if (sqlite3_bind_null(m_stmt, index) != SQLITE_OK)
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::BIND_FAILED, "Failed to bind Null");
		}
		return DatabaseResult::Ok();
	}

	StepResult Statement::Execute()
	{
		int rc = sqlite3_step(m_stmt);
		switch (rc)
		{
		case SQLITE_ROW:
			return StepResult::ROW;
		case SQLITE_DONE:
			return StepResult::DONE;
		default:
			return StepResult::ERROR;
		}
	}

	uint32_t Statement::GetInt32(int column)
	{
		return sqlite3_column_int(m_stmt, column);
	}

	uint64_t Statement::GetInt64(int column)
	{
		return sqlite3_column_int64(m_stmt, column);
	}

	std::string Statement::GetText(int column)
	{
		const unsigned char* text = sqlite3_column_text(m_stmt, column);
		if (text == nullptr)
		{
			return  "";
		}
		return std::string(reinterpret_cast<const char*>(text));
	}

	bool Statement::IsNull(int column)
	{
		return sqlite3_column_type(m_stmt, column) == SQLITE_NULL;
	}

	void Statement::Reset()
	{
		sqlite3_reset(m_stmt);
		sqlite3_clear_bindings(m_stmt);
	}
}
