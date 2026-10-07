
set(PACKET_MONITOR_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_monitor.bpf.c
)
set(HOSTNAME_CAPTURE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/hostname_capture.bpf.c
)

set(PACKET_MONITOR_OBJ
    ${CMAKE_CURRENT_BINARY_DIR}/packet_monitor.bpf.o
)
set(HOSTNAME_CAPTURE_OBJ
    ${CMAKE_CURRENT_BINARY_DIR}/hostname_capture.bpf.o
)

add_custom_command(
    OUTPUT 
        ${PACKET_MONITOR_OBJ}

    COMMAND 
        clang -target bpf -D__TARGET_ARCH_x86 -O2 -g 
        -I${CMAKE_CURRENT_SOURCE_DIR} 
        -I${LIBBPF_INCLUDE_DIRS} 
        -c ${PACKET_MONITOR_SOURCE} 
        -o ${PACKET_MONITOR_OBJ}

    DEPENDS 
        ${PACKET_MONITOR_SOURCE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/vmlinux.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/bpf_network_event.h

    COMMENT "Building packet_monitor BPF"
)

add_custom_command(
    OUTPUT 
        ${HOSTNAME_CAPTURE_OBJ}

    COMMAND 
        clang -target bpf -D__TARGET_ARCH_x86 -O2 -g 
        -I${CMAKE_CURRENT_SOURCE_DIR} 
        -I${LIBBPF_INCLUDE_DIRS} 
        -c ${HOSTNAME_CAPTURE_SOURCE} 
        -o ${HOSTNAME_CAPTURE_OBJ}

    DEPENDS 
        ${HOSTNAME_CAPTURE_SOURCE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/vmlinux.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/bpf_hostname_event.h

    COMMENT "Building hostname_capture BPF"
)

add_custom_target(
    NetScopeBPFPrograms ALL
    DEPENDS ${PACKET_MONITOR_OBJ} ${HOSTNAME_CAPTURE_OBJ}
)

add_custom_command(
    TARGET NetScopeBPFPrograms POST_BUILD

    COMMAND 
        ${CMAKE_COMMAND} 
        -E copy ${PACKET_MONITOR_OBJ}
        ${CMAKE_BINARY_DIR}/app/netscope/
    
    COMMAND 
        ${CMAKE_COMMAND} 
        -E copy ${HOSTNAME_CAPTURE_OBJ}
        ${CMAKE_BINARY_DIR}/app/netscope/

    COMMENT "Copying BPF objects"
)
