add_library(NetScopeFlow STATIC
    "model/IPAddress.h"
    "model/IPAddress.cpp"
    "model/Protocol.h"
    "model/FlowKey.h"
 "model/Endpoint.h" "model/Endpoint.cpp" "model/FlowKey.cpp" "FlowHash.h"
 "model/Flow.h" "model/Flow.cpp" "model/FlowStatistics.h" "storage/FlowTable.cpp"
 "storage/FlowTable.h" "manager/FlowManager.cpp" "manager/FlowManager.h" "model/FlowView.h")

target_include_directories(NetScopeFlow
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeFlow
    PUBLIC
    NetScopeCore
    NetScopePacketCapture
    NetScopeProcessor
)

target_compile_features(NetScopeFlow PUBLIC cxx_std_17)