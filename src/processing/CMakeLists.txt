add_library(NetScopeProcessor STATIC
    "dispatcher/INetworkEventProcessor.h"
    "dispatcher/PacketDispatcher.cpp"
    "coordinator/NetworkCoordinator.cpp" 
    "pipeline/ProcessingPipeline.h" "pipeline/ProcessingPipeline.cpp" 
    "lifecycle/LifecycleScheduler.h" "lifecycle/LifecycleScheduler.cpp"  
  )

target_include_directories(NetScopeProcessor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeProcessor
    PUBLIC
    NetScopeShared
)

target_compile_features(NetScopeProcessor PUBLIC cxx_std_17)