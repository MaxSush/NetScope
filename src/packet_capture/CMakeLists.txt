find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBPF REQUIRED libbpf)

add_subdirectory("bpf")

add_library(NetScopePacketCapture STATIC
    "loader/BpfLoader.cpp"
    "statistics/StatisticsAggregator.cpp"
  
 "service/StatisticsReporter.cpp" 
 "runtime/CaptureRuntime.h" "runtime/CaptureRuntime.cpp")

target_include_directories(NetScopePacketCapture 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBBPF_INCLUDE_DIRS}
)

target_link_libraries(NetScopePacketCapture
    PUBLIC
        NetScopeCore
        NetScopeProcess
        NetScopeFlow
        ${LIBBPF_LIBRARIES}
)

add_dependencies(
    NetScopePacketCapture  PacketMonitorBPF
)

target_compile_features(NetScopePacketCapture PUBLIC cxx_std_17)