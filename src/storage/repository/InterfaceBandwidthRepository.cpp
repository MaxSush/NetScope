#include "InterfaceBandwidthRepository.h"

namespace netscope::storage
{
    InterfaceBandwidthRepository::InterfaceBandwidthRepository(IDatabase& database)
        : m_db(database)
    {
    }

    DatabaseResult InterfaceBandwidthRepository::AccumulateHourly(const std::string& interfaceName, int64_t hourTimestamp, uint64_t txBytes, uint64_t rxBytes, uint64_t txPackets, uint64_t rxPackets)
    {
        constexpr auto Sql = R"sql(
            INSERT INTO bandwidth_interface_hourly
                (interface_name, hour_timestamp, tx_bytes, rx_bytes, tx_packets, rx_packets)
            VALUES (?, ?, ?, ?, ?, ?)
            ON CONFLICT(interface_name, hour_timestamp)
            DO UPDATE SET
                tx_bytes   = tx_bytes   + excluded.tx_bytes,
                rx_bytes   = rx_bytes   + excluded.rx_bytes,
                tx_packets = tx_packets + excluded.tx_packets,
                rx_packets = rx_packets + excluded.rx_packets;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(1, interfaceName);   
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(2, hourTimestamp);    
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(3, txBytes);          
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(4, rxBytes);          
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(5, txPackets);        
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(6, rxPackets);        
        if (!result) 
        {
            return result;
        }

        if (statement->Execute() != StepResult::DONE)
        {
            return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED, "Failed to accumulate interface hourly bandwidth");
        }
        return DatabaseResult::Ok();
    }

    DatabaseResult InterfaceBandwidthRepository::RollUpDaily(int64_t dayTimestamp)
    {
        constexpr auto Sql = R"sql(
            INSERT INTO bandwidth_interface_daily
                (interface_name, day_timestamp, tx_bytes, rx_bytes, tx_packets, rx_packets)
            SELECT
                interface_name, ?,
                SUM(tx_bytes), SUM(rx_bytes),
                SUM(tx_packets), SUM(rx_packets)
            FROM bandwidth_interface_hourly
            WHERE hour_timestamp >= ? AND hour_timestamp < ? + 86400000
            GROUP BY interface_name
            ON CONFLICT(interface_name, day_timestamp)
            DO UPDATE SET
                tx_bytes   = excluded.tx_bytes,
                rx_bytes   = excluded.rx_bytes,
                tx_packets = excluded.tx_packets,
                rx_packets = excluded.rx_packets;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(1, dayTimestamp); 
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(2, dayTimestamp); 
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(3, dayTimestamp); 
        if (!result) 
        {
            return result;
        }

        if (statement->Execute() != StepResult::DONE)
        {
            return DatabaseResult::Failure(DatabaseStatus::FAILED, DatabaseError::EXECUTE_FAILED, "Failed to roll up interface daily bandwidth");
        }
        return DatabaseResult::Ok();
    }

    DatabaseResult InterfaceBandwidthRepository::PurgeHourlyBefore(int64_t cutoffTimestamp)
    {
        constexpr auto Sql = R"sql(
            DELETE FROM bandwidth_interface_hourly WHERE hour_timestamp < ?;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(1, cutoffTimestamp);
        if (!result) 
        {
            return result;
        }

        statement->Execute();
        return DatabaseResult::Ok();
    }

    DatabaseResult InterfaceBandwidthRepository::PurgeDailyBefore(int64_t cutoffTimestamp)
    {
        constexpr auto Sql = R"sql(
            DELETE FROM bandwidth_interface_daily WHERE day_timestamp < ?;
        )sql";

        std::unique_ptr<Statement> statement;
        auto result = m_db.Prepare(Sql, statement);
        if (!result) 
        {
            return result;
        }

        result = statement->Bind(1, cutoffTimestamp);
        if (!result) 
        {
            return result;
        }

        statement->Execute();
        return DatabaseResult::Ok();
    }
}