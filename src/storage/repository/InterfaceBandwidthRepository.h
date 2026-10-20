#pragma once
#include <database/IDatabase.h>
#include <string>

namespace netscope::storage
{
    class InterfaceBandwidthRepository
    {
    public:
        explicit InterfaceBandwidthRepository(IDatabase& database);

        DatabaseResult AccumulateHourly(const std::string& interfaceName, int64_t hourTimestamp, uint64_t txBytes, uint64_t rxBytes, uint64_t txPackets, uint64_t rxPackets);
        DatabaseResult RollUpDaily(int64_t dayTimestamp);

        DatabaseResult PurgeHourlyBefore(int64_t cutoffTimestamp);
        DatabaseResult PurgeDailyBefore(int64_t cutoffTimestamp);

    private:
        IDatabase& m_db;
    };
}