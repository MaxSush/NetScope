add_library(NetScopeProcessor STATIC
    "dispatcher/INetworkEventProcessor.h"
    "dispatcher/PacketDispatcher.cpp"
    
  "activity/FlowUpdatedEvent.h"
  "context/ProcessingContext.h" "coordinator/NetworkCoordinator.cpp" "pipeline/ProcessingPipeline.h" "pipeline/ProcessingPipeline.cpp" "pipeline/IContextProcessor.h" "activity/FlowCreatedEvent.h" "context/LifecycleContext.h" "lifecycle/LifecycleScheduler.h" "lifecycle/LifecycleScheduler.cpp" "lifecycle/ILifecycleProcessor.h" "activity/FlowClosedEvent.h" "activity/ProcessClosedEvent.h")

target_include_directories(NetScopeProcessor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeProcessor
    PUBLIC
    NetScopeCore
    NetScopePacketCapture
    NetScopeProcess
)

target_compile_features(NetScopeProcessor PUBLIC cxx_std_17)