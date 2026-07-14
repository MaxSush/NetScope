#pragma once
#include <database/DatabaseResult.h>

namespace netscope::storage
{
	class IDatabase;

	class SchemaManager
	{
	public:
		DatabaseResult Initialize(IDatabase& db);

	private:
		DatabaseResult CreateTables(IDatabase& db);
		DatabaseResult CreateIndexes(IDatabase& db);

	};
}