#include <gtest/gtest.h>
#include <model/Flow.h>
#include <shared/NetworkEvent.h>
#include <manager/FlowManager.h> 

using namespace netscope::flow;
using namespace netscope::processing;
using namespace netscope;

extern NetworkEvent CreateDummyPacket(pid_t pid);
extern NetworkEvent CreateDummyCloseEvent(CloseReason reason, pid_t pid);

TEST(FlowManagerTests, RoutesCloseEventProperly)
{
    FlowManager manager;

    // 1. Create the flow normally (Simulates the SYN / Data packet)
    NetworkEvent packet = CreateDummyPacket(1234);
    ProcessingContext packetContext(packet);
    manager.Process(packetContext);

    // 2. Send a close event with the same 5-tuple (Simulates the kernel FIN)
    NetworkEvent closeEvent = CreateDummyCloseEvent(CloseReason::TCP_FIN, 1234);

    // FIX: Create a new context specifically for the closeEvent
    ProcessingContext closeContext(closeEvent);

    // Act: The manager should see event.type == CONNECTIONCLOSED and route it
    manager.Process(closeContext);

    // Assert: Check the newly populated closeContext to ensure it routed correctly
    ASSERT_TRUE(closeContext.flow.has_value()); // Safety check so we don't crash the test
    EXPECT_EQ(closeContext.flow->change, FlowChange::CLOSED);
    EXPECT_EQ(closeContext.flow->GetFlow().GetState(), FlowState::CLOSED);
}