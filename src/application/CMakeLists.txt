add_library(NetScopeApplication STATIC
    "NetscopeApplication.cpp"
    
)

target_include_directories(NetScopeApplication
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeApplication
    PUBLIC
    NetScopeProcessor
    NetScopeFlow
    NetScopePresentation
    NetScopePacketCapture
)

target_compile_features(NetScopeApplication PUBLIC cxx_std_17)