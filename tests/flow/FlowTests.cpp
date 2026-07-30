#include <gtest/gtest.h>
#include <chrono>
#include <model/Flow.h>        
#include <shared/NetworkEvent.h> 
#include <storage/FlowTable.h>

using namespace netscope::flow;
using namespace netscope;

inline NetworkEvent CreateDummyPacket()
{
    return NetworkEvent{
        1234,                   // pid
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

TEST(FlowTests, NewFlowStartsActive)
{
    NetworkEvent packet = CreateDummyPacket();

    Endpoint src{ packet.sourceAddress, packet.sourcePort };
    Endpoint dst{ packet.destinationAddress, packet.destinationPort };

    FlowKey key = FlowKey::Create(src, dst, packet.protocol);
    Flow flow(key, packet.pid);

    EXPECT_EQ(flow.GetState(), FlowState::ACTIVE);
}

TEST(FlowTableTests, IdleFlowResumesWhenPacketArrives)
{
    FlowTable table;
    NetworkEvent packet = CreateDummyPacket();

    Endpoint src{ packet.sourceAddress, packet.sourcePort };
    Endpoint dst{ packet.destinationAddress, packet.destinationPort };
    FlowKey key = FlowKey::Create(src, dst, packet.protocol);

    // Act 1: Send first packet (Creates the flow)
    auto [flowRef1, change1] = table.GetOrDefault(key, packet);
    EXPECT_EQ(change1, FlowChange::CREATED);

    // Act 2: Force the flow to IDLE to simulate a timeout
    Flow& mutableFlow = const_cast<Flow&>(flowRef1.get());
    mutableFlow.SetState(FlowState::IDLE);
    EXPECT_EQ(flowRef1.get().GetState(), FlowState::IDLE);

    // Act 3: Send another packet
    auto [flowRef2, change2] = table.GetOrDefault(key, packet);

    // Assert: The Table should catch the IDLE state and upgrade it to RESUMED
    EXPECT_EQ(change2, FlowChange::RESUMED);
    EXPECT_EQ(flowRef2.get().GetState(), FlowState::ACTIVE);
}