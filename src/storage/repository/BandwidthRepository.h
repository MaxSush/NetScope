#pragma once
#include <database/IDatabase.h>

namespace netscope::storage
{
	class BandwidthRepository
	{
	public:
		explicit BandwidthRepository(IDatabase& database);

		DatabaseResult AccumulateHourly(int64_t processId, int64_t hourTimestamp,
			uint64_t uploadBytes, uint64_t downloadBytes, uint32_t flowCount);

		DatabaseResult RollUpDaily(int64_t dayTimestamp);

		DatabaseResult PurgeHourlyBefore(int64_t cutoffTimestamp);
		DatabaseResult PurgeDailyBefore(int64_t cutoffTimestamp);

	private:
		IDatabase& m_db;
	};
}