add_library(NetScopeShared STATIC
    "model/IPAddress.h"
    "model/IPAddress.cpp"
    "model/Protocol.h"
    "model/FlowKey.h" "model/FlowKey.cpp"
    "model/Endpoint.h" "model/Endpoint.cpp" 
    "model/FlowStatistics.h" 
    "model/FlowSnapshot.h"  
    "model/FlowResult.h" 
    "model/FlowState.h"
    "model/ProcessInfo.h"
    "model/ProcessResult.h"
    "activity/FlowClosedEvent.h" "activity/ProcessClosedEvent.h" "activity/FlowCreatedEvent.h" "activity/FlowUpdatedEvent.h"
    "pipeline/LifecycleContext.h" "pipeline/ProcessingContext.h" 
    "pipeline/ILifecycleProcessor.h" "pipeline/IContextProcessor.h"
)

target_include_directories(NetScopeShared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeShared
    PUBLIC
    NetScopeCore
)

target_compile_features(NetScopeShared PUBLIC cxx_std_17)