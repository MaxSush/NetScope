add_executable (NetScope   
	"main.cpp"
)

target_include_directories(NetScope
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(NetScope PRIVATE
    NetScopeCore
    NetScopeProcess
    NetScopePacketCapture
    NetScopeFlow
)

target_compile_options(NetScope PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

