add_library(NetScopePresentation STATIC
    "console/FlowReporter.cpp"
  "console/ProcessReporter.h" "console/ProcessReporter.cpp"
    "aggregation/ProcessAggregator.h" "aggregation/ProcessAggregator.cpp"
)

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