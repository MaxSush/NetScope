#include <gtest/gtest.h>
#include <chrono>
#include <model/Flow.h>
#include <shared/NetworkEvent.h>
#include <coordinator/NetworkCoordinator.h>
#include <context/ProcessingContext.h>
#include <context/LifecycleContext.h>
#include <event/IEventSubscriber.h>

// Note: Adjust these include paths if your event structs live in a different folder
#include <activity/FlowCreatedEvent.h> 
#include <activity/FlowUpdatedEvent.h>
#include <activity/FlowClosedEvent.h>

using namespace netscope::processing;
using namespace netscope::flow;
using namespace netscope::core;
using namespace netscope;

// Helper to generate a packet
inline NetworkEvent CreateDummyPacket(pid_t pid = 1234)
{
    return NetworkEvent{
        pid,                                 // pid
        1235,                                // tid
        100,                                 // bytes
        NetworkEventType::PACKET,            // type
        std::nullopt,                        // closeReason
        flow::Protocol::TCP,                 // protocol
        Direction::OUTGOING,                 // direction
        flow::AddressFamily::IPv4,           // family
        8080,                                // sourcePort
        443,                                 // destinationPort
        flow::IPAddress::IPv4(0xC0A80101),   // sourceAddress (192.168.1.1)
        flow::IPAddress::IPv4(0x08080808)    // destinationAddress (8.8.8.8)
    };
}

// And the new close event helper based on the updated struct
inline NetworkEvent CreateDummyCloseEvent(CloseReason reason = CloseReason::TCP_FIN, pid_t pid = 1234)
{
    NetworkEvent event = CreateDummyPacket(pid);

    // Override the packet defaults to represent a closed event
    event.type = NetworkEventType::CONNECTIONCLOSED;
    event.closeReason = reason;
    event.bytes = 0; // A pure state-change event doesn't carry payload bytes

    return event;
}

// --- Mock Subscribers ---

class MockCreatedSubscriber : public IEventSubscriber<FlowCreatedEvent>
{
public:
    int count = 0;
    void HandleEvent(const FlowCreatedEvent& event) override { ++count; }
};

class MockUpdatedSubscriber : public IEventSubscriber<FlowUpdatedEvent>
{
public:
    int count = 0;
    void HandleEvent(const FlowUpdatedEvent& event) override { ++count; }
};

class MockClosedSubscriber : public IEventSubscriber<FlowClosedEvent>
{
public:
    int count = 0;
    void HandleEvent(const FlowClosedEvent& event) override { ++count; }
};

// --- Tests for ProcessingContext (Data-Driven / Packets) ---

TEST(NetworkCoordinatorTests, ProcessingContext_PublishesFlowCreated)
{
    NetworkCoordinator coordinator;
    MockCreatedSubscriber mockCreated;
    coordinator.Subscribe(mockCreated);

    NetworkEvent dummyInput = CreateDummyPacket();
    ProcessingContext context(dummyInput);

    Endpoint src{ dummyInput.sourceAddress, dummyInput.sourcePort };
    Endpoint dst{ dummyInput.destinationAddress, dummyInput.destinationPort };
    Flow dummyFlow(FlowKey::Create(src, dst, dummyInput.protocol), dummyInput.pid);

    // Simulate FlowManager returning CREATED
    context.flow = FlowResult{ std::cref(dummyFlow), FlowChange::CREATED };

    coordinator.Coordinate(context);

    EXPECT_EQ(mockCreated.count, 1);
}

TEST(NetworkCoordinatorTests, ProcessingContext_PublishesFlowUpdatedOnUpdateAndResume)
{
    NetworkCoordinator coordinator;
    MockUpdatedSubscriber mockUpdated;
    coordinator.Subscribe(mockUpdated);

    NetworkEvent dummyInput = CreateDummyPacket();
    Endpoint src{ dummyInput.sourceAddress, dummyInput.sourcePort };
    Endpoint dst{ dummyInput.destinationAddress, dummyInput.destinationPort };
    Flow dummyFlow(FlowKey::Create(src, dst, dummyInput.protocol), dummyInput.pid);

    // Test 1: UPDATED
    ProcessingContext contextUpdate(dummyInput);
    contextUpdate.flow = FlowResult{ std::cref(dummyFlow), FlowChange::UPDATED };
    coordinator.Coordinate(contextUpdate);
    EXPECT_EQ(mockUpdated.count, 1);

    // Test 2: RESUMED
    ProcessingContext contextResume(dummyInput);
    contextResume.flow = FlowResult{ std::cref(dummyFlow), FlowChange::RESUMED };
    coordinator.Coordinate(contextResume);
    EXPECT_EQ(mockUpdated.count, 2); // Should increment again!
}

// --- Tests for LifecycleContext (Time-Driven / Scheduler) ---

TEST(NetworkCoordinatorTests, LifecycleContext_PublishesFlowUpdatedOnIdle)
{
    NetworkCoordinator coordinator;
    MockUpdatedSubscriber mockUpdated;
    coordinator.Subscribe(mockUpdated);

    LifecycleContext context(std::chrono::steady_clock::now());

    NetworkEvent dummyInput = CreateDummyPacket();
    Endpoint src{ dummyInput.sourceAddress, dummyInput.sourcePort };
    Endpoint dst{ dummyInput.destinationAddress, dummyInput.destinationPort };
    Flow dummyFlow(FlowKey::Create(src, dst, dummyInput.protocol), dummyInput.pid);

    // Simulate the Lifecycle thread changing a flow to IDLE
    context.flowChanges.push_back(FlowResult{ std::cref(dummyFlow), FlowChange::IDLE });

    coordinator.Coordinate(context);

    // An IDLE change should be broadcasted as an Update so the DB can save the sleeping state
    EXPECT_EQ(mockUpdated.count, 1);
}

//TEST(NetworkCoordinatorTests, LifecycleContext_PublishesFlowClosedOnArchive)
//{
//    NetworkCoordinator coordinator;
//    MockClosedSubscriber mockClosed;
//    coordinator.Subscribe(mockClosed);
//
//    LifecycleContext context(std::chrono::steady_clock::now());
//
//    NetworkEvent dummyInput = CreateDummyPacket();
//    Endpoint src{ dummyInput.sourceAddress, dummyInput.sourcePort };
//    Endpoint dst{ dummyInput.destinationAddress, dummyInput.destinationPort };
//    Flow dummyFlow(FlowKey::Create(src, dst, dummyInput.protocol), dummyInput.pid);
//
//    // Simulate the Lifecycle thread confirming archival
//    context.flowChanges.push_back(FlowResult{ std::cref(dummyFlow), FlowChange::ARCHIVED });
//
//    coordinator.Coordinate(context);
//
//    // An ARCHIVED change should be broadcasted as Closed so the DB finalizes the row
//    EXPECT_EQ(mockClosed.count, 1);
//}

TEST(NetworkCoordinatorTests, ProcessingContext_PublishesFlowClosedEvent)
{
    NetworkCoordinator coordinator;
    MockClosedSubscriber mockClosed;
    coordinator.Subscribe(mockClosed);

    NetworkEvent dummyClose = CreateDummyCloseEvent(CloseReason::TCP_RESET);
    ProcessingContext context(dummyClose);

    Endpoint src{ dummyClose.sourceAddress, dummyClose.sourcePort };
    Endpoint dst{ dummyClose.destinationAddress, dummyClose.destinationPort };
    Flow dummyFlow(FlowKey::Create(src, dst, dummyClose.protocol), dummyClose.pid);

    // Simulate FlowManager successfully returning the CLOSED state
    context.flow = FlowResult{ std::cref(dummyFlow), FlowChange::CLOSED };

    // Act
    coordinator.Coordinate(context);

    // Assert
    EXPECT_EQ(mockClosed.count, 1);
}