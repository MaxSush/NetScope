#include "SQLiteDatabase.h"
#include <sqlite3.h>

namespace netscope::storage
{
	SQLiteDatabase::SQLiteDatabase()
	{
	}

	SQLiteDatabase::~SQLiteDatabase()
	{
		Close();
	}

	DatabaseResult SQLiteDatabase::Open(const std::string& path)
	{
		if (m_db != nullptr)
		{
			return DatabaseResult::Ok();
		}

		int result = sqlite3_open(path.c_str(), &m_db);
		if (result != SQLITE_OK)
		{
			Close();
			std::string msg = sqlite3_errmsg(m_db);
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::OPEN_FAILED, "SQLiteDatabase::Open|DatabaseError::OPEN_FAILED|" + msg);
		}

		return DatabaseResult::Ok();
	}

	void SQLiteDatabase::Close()
	{
		if (m_db != nullptr)
		{
			sqlite3_close(m_db);
			m_db = nullptr;
		}
	}

	bool SQLiteDatabase::IsOpen() const
	{
		return m_db != nullptr;
	}

	DatabaseResult SQLiteDatabase::Execute(const std::string& sql)
	{
		if (!IsOpen())
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::NOT_OPEN, "Database is not opened.");
		}
		char* error_msg = nullptr;

		int result = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &error_msg);

		if (result != SQLITE_OK)
		{
			std::string msg = (error_msg != nullptr) ? error_msg : "Unknown SQLITE error.";
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED, sql + " | " + msg);
		}

		return DatabaseResult::Ok();
	}

	DatabaseResult SQLiteDatabase::Prepare(const std::string& sql, std::unique_ptr<Statement>& statement)
	{
		if (!IsOpen())
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::NOT_OPEN, "Database is not opened.");
		}

		sqlite3_stmt* stmt = nullptr;

		int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);

		if (rc != SQLITE_OK)
		{
			return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::PREPARE_FAILED, std::string("Failed to prepare statement: ") + sqlite3_errmsg(m_db));
		}

		statement = std::make_unique<Statement>(stmt);
		
		return DatabaseResult::Ok();
	}

	DatabaseResult SQLiteDatabase::BeginTransaction()
	{
		return Execute("BEGIN TRANSACTION;");
	}

	DatabaseResult SQLiteDatabase::Commit()
	{
		return Execute("COMMIT;");
	}

	DatabaseResult SQLiteDatabase::Rollback()
	{
		return Execute("ROLLBACK;");
	}
}