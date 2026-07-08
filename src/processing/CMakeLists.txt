add_library(NetScopeProcessor STATIC
    "dispatcher/INetworkEventProcessor.h"
    "dispatcher/PacketDispatcher.cpp"
    
)

target_include_directories(NetScopeProcessor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeProcessor
    PUBLIC
    NetScopeCore
    NetScopePacketCapture
)

target_compile_features(NetScopeProcessor PUBLIC cxx_std_17)