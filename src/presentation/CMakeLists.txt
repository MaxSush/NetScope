add_library(NetScopePresentation STATIC
    
   
    "aggregation/ProcessAggregator.h" "aggregation/ProcessAggregator.cpp"
 "console/ConsoleReporter.h" "console/ConsoleReporter.cpp")

target_include_directories(NetScopePresentation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopePresentation
    PUBLIC
    NetScopeFlow
    NetScopeProcess
)

target_compile_features(NetScopePresentation PUBLIC cxx_std_17)