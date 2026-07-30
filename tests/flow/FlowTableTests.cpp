#include <gtest/gtest.h>
#include <chrono>
#include <model/Flow.h>
#include <shared/NetworkEvent.h>
#include <storage/FlowTable.h> 
#include <context/LifecycleContext.h>

using namespace netscope::flow;
using namespace netscope::processing;
using namespace netscope;

// Helper to generate a packet (Matches FlowTests exactly, with optional PID override)
inline NetworkEvent CreateDummyPacket(pid_t pid = 1234)
{
    return NetworkEvent{
        pid,                    // pid
        1235,                   // ppid
        100,                    // length/bytes
        Protocol::TCP,          // protocol
        Direction::OUTGOING,    // direction
        AddressFamily::IPv4,    // family
        8080,                   // sourcePort
        443,                    // destinationPort
        IPAddress::IPv4(0xC0A80101), // sourceAddress (192.168.1.1)
        IPAddress::IPv4(0x08080808)  // destinationAddress (8.8.8.8)
    };
}

TEST(FlowTableTests, NewFlowReturnsCreated)
{
    FlowTable table;
    NetworkEvent packet = CreateDummyPacket();
    Endpoint src{ packet.sourceAddress, packet.sourcePort };
    Endpoint dst{ packet.destinationAddress, packet.destinationPort };
    FlowKey key = FlowKey::Create(src, dst, packet.protocol);

    auto [flow, change] = table.GetOrDefault(key, packet);

    EXPECT_EQ(change, FlowChange::CREATED);
    EXPECT_EQ(flow.get().GetState(), FlowState::ACTIVE);
}

TEST(FlowTableTests, ExistingFlowReturnsUpdated)
{
    FlowTable table;
    NetworkEvent packet = CreateDummyPacket();
    Endpoint src{ packet.sourceAddress, packet.sourcePort };
    Endpoint dst{ packet.destinationAddress, packet.destinationPort };
    FlowKey key = FlowKey::Create(src, dst, packet.protocol);

    table.GetOrDefault(key, packet);

    // Second packet hits the same key
    FlowResult result = table.GetOrDefault(key, packet);
    EXPECT_EQ(result.change, FlowChange::UPDATED);
}

TEST(FlowTableTests, IdleFlowReturnsResumed)
{
    FlowTable table;
    NetworkEvent packet = CreateDummyPacket();
    Endpoint src{ packet.sourceAddress, packet.sourcePort };
    Endpoint dst{ packet.destinationAddress, packet.destinationPort };
    FlowKey key = FlowKey::Create(src, dst, packet.protocol);

    // 1. Create it
    table.GetOrDefault(key, packet);

    // 2. Force it to IDLE (simulating the lifecycle thread)
    Flow* mutableFlow = table.Find(key);
    ASSERT_NE(mutableFlow, nullptr);
    mutableFlow->SetState(FlowState::IDLE);

    // 3. Send another packet
    FlowResult resumedResult = table.GetOrDefault(key, packet);

    // 4. Verify it woke up properly
    EXPECT_EQ(resumedResult.change, FlowChange::RESUMED);
    EXPECT_EQ(resumedResult.GetFlow().GetState(), FlowState::ACTIVE);
}

TEST(FlowTableTests, LifecycleTickMarksFlowIdle)
{
    FlowTable table;
    NetworkEvent packet = CreateDummyPacket();
    Endpoint src{ packet.sourceAddress, packet.sourcePort };
    Endpoint dst{ packet.destinationAddress, packet.destinationPort };
    FlowKey key = FlowKey::Create(src, dst, packet.protocol);

    table.GetOrDefault(key, packet);

    // Simulate time passing way beyond the idle timeout (e.g., 2 minutes in the future)
    auto futureTime = std::chrono::steady_clock::now() + std::chrono::seconds(120);
    LifecycleContext context(futureTime);

    // Run the lifecycle tick
    table.UpdateFlows(context);

    // Verify the context captured the state change
    ASSERT_EQ(context.flowChanges.size(), 1);
    EXPECT_EQ(context.flowChanges[0].change, FlowChange::IDLE);
    EXPECT_EQ(context.flowChanges[0].GetFlow().GetState(), FlowState::IDLE);
}

TEST(FlowTableTests, CleanUpArchivedFlowsRemovesArchivedOnly)
{
    FlowTable table;

    // Create two distinct packets by overriding ports so the FlowKeys are different
    NetworkEvent packet1 = CreateDummyPacket();
    packet1.sourcePort = 10001;
    Endpoint src1{ packet1.sourceAddress, packet1.sourcePort };
    Endpoint dst1{ packet1.destinationAddress, packet1.destinationPort };
    FlowKey key1 = FlowKey::Create(src1, dst1, packet1.protocol);

    NetworkEvent packet2 = CreateDummyPacket();
    packet2.sourcePort = 10002;
    Endpoint src2{ packet2.sourceAddress, packet2.sourcePort };
    Endpoint dst2{ packet2.destinationAddress, packet2.destinationPort };
    FlowKey key2 = FlowKey::Create(src2, dst2, packet2.protocol);

    table.GetOrDefault(key1, packet1);
    table.GetOrDefault(key2, packet2);

    // Mark flow1 as ARCHIVED, leave flow2 ACTIVE
    Flow* mutableFlow1 = table.Find(key1);
    ASSERT_NE(mutableFlow1, nullptr);
    mutableFlow1->SetState(FlowState::ARCHIVED);

    // Run cleanup
    table.CleanUpArchivedFlows();

    // Verify table structure
    FlowResult checkFlow2 = table.GetOrDefault(key2, packet2);
    EXPECT_EQ(checkFlow2.change, FlowChange::UPDATED); // flow2 survived!

    FlowResult checkDeletedFlow1 = table.GetOrDefault(key1, packet1);
    EXPECT_EQ(checkDeletedFlow1.change, FlowChange::CREATED); // flow1 was deleted, so this is a new creation!
}