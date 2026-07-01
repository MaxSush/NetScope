add_library(NetScopeProcess STATIC
    "ProcessManager.cpp"
    "SocketResolver.cpp"
    "ConnectionTable.cpp" "CachedProcess.h"
 "ProcessCache.h" "ProcessCache.cpp")

target_include_directories(NetScopeProcess
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(NetScopeProcess PUBLIC cxx_std_17)