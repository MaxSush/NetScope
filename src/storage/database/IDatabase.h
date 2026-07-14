#pragma once

#include <string>
#include "DatabaseResult.h"
#include <memory>
#include "Statement.h"

namespace netscope::storage
{
	class IDatabase
	{
	public:
		virtual ~IDatabase() = default;

		virtual DatabaseResult Open(const std::string& path) = 0;
		virtual void Close() = 0;
		virtual bool IsOpen() const = 0;

		virtual DatabaseResult Execute(const std::string& sql) = 0;
		virtual DatabaseResult Prepare(const std::string& sql, std::unique_ptr<Statement>& statement) = 0;

		virtual DatabaseResult BeginTransaction() = 0;
		virtual DatabaseResult Commit() = 0;
		virtual DatabaseResult Rollback() = 0;
	};
}