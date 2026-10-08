add_library(NetScopeProcess STATIC
    "ProcessManager.cpp"
    "SocketResolver.cpp"
)

 target_include_directories(NetScopeProcess
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeProcess
    PUBLIC
    NetScopeShared
)

target_compile_features(NetScopeProcess PUBLIC cxx_std_17)