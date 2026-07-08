add_executable (NetScope   
	"main.cpp"
)

target_include_directories(NetScope
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(NetScope PRIVATE
    NetScopeApplication
    NetScopeCore
    NetScopeFlow
    NetScopePacketCapture
    NetScopePresentation
    NetScopeProcess
    NetScopeProcessor
)

target_compile_options(NetScope PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

