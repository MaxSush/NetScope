add_library(NetScopeApplication STATIC
    "NetscopeApplication.cpp"
    "DebugSubscriber.h"
)

target_include_directories(NetScopeApplication
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeApplication
    PUBLIC
    NetScopeCore
    NetScopeProcessor
    NetScopeFlow
    NetScopePresentation
    NetScopePacketCapture
    NetScopeStorage
)

target_compile_features(NetScopeApplication PUBLIC cxx_std_17)