add_library(NetScopeFlow STATIC
    "model/Flow.h" "model/Flow.cpp"  
    "storage/FlowTable.cpp" "storage/FlowTable.h" 
    "manager/FlowManager.cpp" "manager/FlowManager.h"
)

target_include_directories(NetScopeFlow
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeFlow
    PUBLIC
    NetScopeShared
)

target_compile_features(NetScopeFlow PUBLIC cxx_std_17)