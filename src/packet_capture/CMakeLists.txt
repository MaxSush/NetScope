find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBPF REQUIRED libbpf)

add_subdirectory("bpf")

add_library(NetScopePacketCapture STATIC
    "loader/BpfLoader.cpp"
    "statistics/StatisticsAggregator.cpp"
 "service/PacketCaptureService.cpp" 
 "service/StatisticsReporter.cpp" "dispatcher/PacketDispatcher.cpp"
 "dispatcher/PacketDispatcher.h" "dispatcher/INetworkEventProcessor.h")

target_include_directories(NetScopePacketCapture 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBBPF_INCLUDE_DIRS}
)

target_link_libraries(NetScopePacketCapture
    PRIVATE
        NetScopeCore
        NetScopeProcess
        NetScopeFlow
        ${LIBBPF_LIBRARIES}
)

add_dependencies(
    NetScopePacketCapture  PacketMonitorBPF
)

target_compile_features(NetScopePacketCapture PUBLIC cxx_std_17)