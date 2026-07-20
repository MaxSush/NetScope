add_library(NetScopeProcess STATIC
    "ProcessManager.cpp"
    "SocketResolver.cpp"
    "ConnectionTable.cpp" 
  "ProcessResult.h")

 target_include_directories(NetScopeProcess
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeProcess
    PUBLIC
    NetScopeProcessor
)

target_compile_features(NetScopeProcess PUBLIC cxx_std_17)