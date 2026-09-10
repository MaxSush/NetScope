#pragma once

#include <database/IDatabase.h>
#include <optional>

namespace netscope::storage
{
	class AppStateRepository
	{
	public:
		explicit AppStateRepository(IDatabase& database);

		DatabaseResult Set(const std::string& key, const std::string& value);
		std::optional<std::string> Get(const std::string& key);

	private:
		IDatabase& m_db;
	};
}