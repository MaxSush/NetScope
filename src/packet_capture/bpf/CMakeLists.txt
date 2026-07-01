
set(BPF_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_monitor.bpf.c
)

set(BPF_OBJECT
    ${CMAKE_CURRENT_BINARY_DIR}/packet_monitor.bpf.o
)

add_custom_command(
    OUTPUT ${BPF_OBJECT}

    COMMAND clang -target bpf -D__TARGET_ARCH_x86 -O2 -g -I${CMAKE_CURRENT_SOURCE_DIR} -I${LIBBPF_INCLUDE_DIRS} -c ${BPF_SOURCE} -o ${BPF_OBJECT}

    DEPENDS ${BPF_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/vmlinux.h ${CMAKE_CURRENT_SOURCE_DIR}/packet_event.h

    COMMENT "Building BPF programs"
)

add_custom_target(
    PacketMonitorBPF
    DEPENDS ${BPF_OBJECT}
)

add_custom_command(
    TARGET PacketMonitorBPF
    POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy
        ${BPF_OBJECT}
        ${CMAKE_BINARY_DIR}/app/netscope/

    COMMENT "Moving BPF programs"
)
