#pragma once

#include <string>
#include <cstdint>
#include "DatabaseResult.h"

struct sqlite3_stmt;

namespace netscope::storage
{
	enum class StepResult
	{
		ROW,
		DONE,
		ERROR
	};

	class Statement
	{
	public:
		explicit Statement(sqlite3_stmt* statement);

		~Statement();

		Statement(const Statement&) = delete;
		Statement& operator=(const Statement&) = delete;

		Statement(Statement&& other) noexcept;
		Statement& operator=(Statement&& other) noexcept;

		DatabaseResult Bind(int index, uint32_t val);
		DatabaseResult Bind(int index, uint64_t val);
		DatabaseResult Bind(int index, int64_t val);
		DatabaseResult Bind(int index, int32_t val);
		DatabaseResult Bind(int index, const std::string& val);
		DatabaseResult BindNull(int index);

		StepResult Execute();

		uint32_t GetInt32(int column);
		uint64_t GetInt64(int column);
		std::string GetText(int column);
		bool IsNull(int column);

		void Reset();

	private:
		sqlite3_stmt* m_stmt{};
	};
}