add_executable (NetScope   
	"main.cpp"
)

target_include_directories(NetScope
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(NetScope PRIVATE
    NetScopeCore
    NetScopeProcess
    NetScopePacketCapture
)

target_compile_options(NetScope PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

