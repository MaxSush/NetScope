#pragma once

#include "IDatabase.h"
#include <memory>
#include "Statement.h"

struct sqlite3;

namespace netscope::storage
{
	class SQLiteDatabase : public IDatabase
	{
	public:
		SQLiteDatabase();
		~SQLiteDatabase() override;

		DatabaseResult Open(const std::string& path) override;
		void Close() override;
		bool IsOpen() const override;

		DatabaseResult Execute(const std::string& sql) override;

		DatabaseResult Prepare(const std::string& sql, std::unique_ptr<Statement>& statement) override;

		DatabaseResult BeginTransaction() override;
		DatabaseResult Commit() override;
		DatabaseResult Rollback() override;

	private:
		sqlite3* m_db = nullptr;

	};
}