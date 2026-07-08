# NetScope

> A high-performance Linux network monitoring application that attributes network traffic to individual processes using **eBPF** and modern **C++17**.

**Milestone 1 Status:** ✅ Core Monitoring Engine Complete

---

## Overview

NetScope is a system-level network monitoring application built from scratch to understand the complete networking stack rather than relying on existing monitoring tools.

The project captures network events inside the Linux kernel using **eBPF**, transfers them to user space through a **Ring Buffer**, processes them using a multi-threaded event-driven architecture, groups packets into network flows, aggregates statistics per process, and presents live network usage in the console.

The architecture is designed with future extensibility in mind, allowing additional frontends such as **Qt**, persistent storage using **SQLite**, and historical analytics without modifying the core packet processing pipeline.

---

## Milestone 1 Features

- eBPF based packet monitoring
- Ring Buffer communication between kernel and user space
- Event-driven packet processing pipeline
- Multi-threaded architecture
- Flow-based network tracking
- Automatic idle flow cleanup
- Process-aware traffic aggregation
- Thread-safe snapshot system
- Live console reporting
- Modular architecture for future expansion

---

## Architecture

```
                        Linux Kernel
                             │
                             ▼
                     eBPF Packet Hooks
                             │
                             ▼
                        Ring Buffer
                             │
                             ▼
                     CaptureRuntime
                             │
                             ▼
                        EventQueue
                             │
                             ▼
                    PacketDispatcher
                             │
                             ▼
                       FlowManager
                             │
                             ▼
                        FlowTable
                             │
                     Thread-safe Snapshot
                             │
                             ▼
                         FlowView
                             │
                             ▼
                    ProcessAggregator
                             │
                             ▼
                        ProcessView
                             │
                             ▼
                     Console Reporter
```

---

## Technologies Used

- C++17
- eBPF
- libbpf
- Linux Ring Buffer
- Ubuntu 24.04.4 LTS Noble (Development)
- CMake
- spdlog
- fmt
- Modern STL

---

## Current Console Output

### Timepoint 1

```
[2026-07-08 22:28:36.770] [warning] Removed Idle Connections: 0

PID      PROCESS                ACTIVE     IDLE          UPLOAD        DOWNLOAD
------------------------------------------------------------------------------------------------------
14166    [Terminated]                1        0              73             773
14129    sshd                        1        0            3219            1627
14107    sshd                        1        0            8051            4459
2125     sshd                        1        0           18064              96
14165    wget                        1        0            1550          163126
```

### Timepoint 2

```
[2026-07-08 22:28:52.926] [warning] Removed Idle Connections: 1

PID      PROCESS                ACTIVE     IDLE          UPLOAD        DOWNLOAD
------------------------------------------------------------------------------------------------------
14166    [Terminated]                1        0              73             773
14107    sshd                        1        0            8051            4459
2125     sshd                        1        0           29792            8544
```

### Timepoint 3

```
[2026-07-08 22:30:01.631] [warning] Removed Idle Connections: 1

PID      PROCESS                ACTIVE     IDLE          UPLOAD        DOWNLOAD
------------------------------------------------------------------------------------------------------
2125     sshd                        1        0           62320            8640
```

---

## Project Structure

```
NetScope
├── application
├── core
│   ├── event
│   ├── logger
│   ├── threading
│   └── timer
├── packet_capture
│   ├── bpf
│   ├── loader
│   └── runtime
├── flow
│   ├── manager
│   ├── model
│   └── storage
├── process
├── processing
│   ├── aggregation
│   └── dispatcher
└── presentation
    ├── console
    └── model
```

---

## Current Capabilities

- Monitor live network activity
- Attribute traffic to Linux processes
- Aggregate upload/download statistics
- Track active network flows
- Automatically remove idle flows
- Thread-safe read/write architecture
- Modular processing pipeline

---

## Future Scope

Upcoming milestones will include:

- SQLite persistence
- Historical traffic database
- Runtime configuration system
- Graceful application lifecycle management
- Qt desktop dashboard
- Real-time charts and graphs
- Flow filtering and search
- Protocol-wise statistics
- Export to CSV/JSON
- Performance optimizations
- Unit and integration testing

---

## Learning Objectives

This project focuses on learning and implementing:

- Modern C++ design
- System programming
- Linux networking
- eBPF development
- Multi-threading
- Concurrent programming
- Software architecture
- High-performance data processing
- Monitoring systems

---

## Milestone Status

| Milestone | Status |
|-----------|--------|
| Core Infrastructure | ✅ |
| Process Database | ✅ |
| eBPF Packet Capture | ✅ |
| Flow Engine | ✅ |
| Flow Aggregation & Reporting | ✅ |
| Persistence Layer | 🚧 |
| Desktop UI | ⏳ |

---

## License

This project is developed for educational purposes to explore Linux systems programming, eBPF, and high-performance software architecture.