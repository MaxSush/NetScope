# NetScope CodeMap

## 1. Layer Hierarchy & Dependency Direction

The NetScope architecture consists of the following layers, mapped directly to their CMake targets. Dependencies are extracted from each module's `CMakeLists.txt`:

*   **core** (`NetScopeCore`)
    *   **Dependencies:** `PRIVATE fmt::fmt`, `PRIVATE spdlog::spdlog`, `PRIVATE tomlplusplus::tomlplusplus`
*   **hostname** (`NetscopeHostname`)
    *   **Dependencies:** `PUBLIC NetScopeCore`, `PUBLIC NetScopeFlow`
*   **packet_capture** (`NetScopePacketCapture`)
    *   **Dependencies:** `PUBLIC NetScopeCore`, `PUBLIC NetScopeFlow`, `PUBLIC NetscopeHostname`, `PUBLIC ${LIBBPF_LIBRARIES}`
*   **process** (`NetScopeProcess`)
    *   **Dependencies:** `PUBLIC NetScopeProcessor`
*   **processing** (`NetScopeProcessor`)
    *   **Dependencies:** `PUBLIC NetScopeCore`, `PUBLIC NetScopePacketCapture`, `PUBLIC NetScopeProcess`
*   **flow** (`NetScopeFlow`)
    *   **Dependencies:** `PUBLIC NetScopeCore`, `PUBLIC NetScopePacketCapture`, `PUBLIC NetScopeProcessor`
*   **presentation** (`NetScopePresentation`)
    *   **Dependencies:** `PUBLIC NetScopeFlow`, `PUBLIC NetScopeProcess`
*   **storage** (`NetScopeStorage`)
    *   **Dependencies:** `PRIVATE SQLite::SQLite3`, `PRIVATE NetScopeProcess`, `PRIVATE NetScopeCore`, `PRIVATE NetScopeFlow`
*   **application** (`NetScopeApplication`)
    *   **Dependencies:** `PUBLIC NetScopeCore`, `PUBLIC NetScopeProcessor`, `PUBLIC NetScopeFlow`, `PUBLIC NetScopePresentation`, `PUBLIC NetScopePacketCapture`, `PUBLIC NetScopeStorage`
*   **app** (`NetScope` - The entrypoint executable)
    *   **Dependencies:** `PRIVATE NetScopeApplication`, `PRIVATE NetScopeCore`, `PRIVATE NetScopeFlow`, `PRIVATE NetScopePacketCapture`, `PRIVATE NetScopePresentation`, `PRIVATE NetScopeProcess`, `PRIVATE NetScopeProcessor`

## 2. Module Interfaces, Class Contracts, and Method Workings

### Core (`src/core`)
*   `Config`: Global configuration properties container.
*   `ConfigLoader`: 
    *   `static Config Load()`: Loads configuration from `/etc/netscope/config.toml`, then overrides with user's `~/.config/netscope/config.toml`, and finally any CLI override. Returns merged `Config`.
*   `DomainEventBus<T>` / `EventBus`:
    *   `Subscribe()`: Adds a callback/subscriber to a strongly-typed event list.
    *   `Publish()`: Iterates through subscribers and invokes their handlers synchronously.
*   `EventQueue<T>`:
    *   `Push()` / `Pop()` / `PopFor()`: Thread-safe locking queue using `std::mutex` and `std::condition_variable` for safe cross-thread event passing.
*   `Thread`: 
    *   `Start()`: Spawns a `std::thread` executing the provided function, catching exceptions, and managing running state.
    *   `RequestStop()` / `Join()`: Signals atomic stop flags and joins the native thread.
*   `Timer`:
    *   `ElapsedSeconds()`, `ElapsedMilliSeconds()`: Computes time deltas using `std::chrono::steady_clock`.

### Flow (`src/flow`)
*   `Flow`:
    *   `Update(const NetworkEvent&)`: Increments byte/packet counters depending on `Direction::OUTGOING` vs `INCOMMING`. Updates `last_seen` and `last_activity`.
    *   `GetSnapshot()`: Generates a lightweight, immutable copy (`FlowSnapshot`) of current state (counters, endpoints, states) for thread-safe consumption.
    *   `IsIdle()` / `HasTcpIdleTimeout()`: Compares current time against `last_activity` using configured timeout thresholds.
*   `FlowManager` (Implements `IContextProcessor`, `ILifecycleProcessor`):
    *   `Process(ProcessingContext&)`: Evaluates a real-time `NetworkEvent`. Depending on the type (`PACKET`, `CONNECTION_CLOSED`, `PROCESS_EXITED`), it calls the appropriate `FlowTable` method to upsert or close flows.
    *   `Process(LifecycleContext&)`: Triggers idle timeout detection for flows in the table.
    *   `CleanUpExpired()`: Purges totally closed/evicted flows from memory to prevent memory leaks.
*   `FlowTable`: 
    *   Thread-safe internal cache using `std::shared_mutex`.
    *   `GetOrDefault()`: Finds a flow by `FlowKey` or initializes a new one. 
    *   `CloseFlowsByPid()`: Iterates through all tracked flows matching a PID and flags them as `CLOSED`.

### Hostname (`src/hostname`)
*   `DnsCache`:
    *   `Insert()` / `Lookup()`: Thread-safe cache matching IPs to hostnames. Uses an LRU list (`std::list`) mapped with `std::unordered_map` to enforce max entries.
*   `RdnsResolver`:
    *   `Run()` / `Resolve()`: Background thread popping from a queue to perform `getnameinfo()` lookups on IPs missing from cache, inserting the results back into `DnsCache`.
*   `DnsResponseParser` / `SniParser`:
    *   `Parse()`: Binary parsers reading raw packet payloads to extract DNS domains or TLS SNI hostnames.

### Packet Capture (`src/packet_capture`)
*   `BPFLoader`:
    *   `Load()`: Loads `packet_monitor.bpf.o` into kernel via `libbpf`, attaches tracepoints/kprobes to `tcp_sendmsg`, `tcp_recvmsg`, etc. Configures the BPF Ring Buffer callback.
    *   `Poll()`: Interrogates the ring buffer, triggering `HandleEvent` which unpacks raw kernel structs (`bpf_network_event`) into Normalized `NetworkEvent`s, evaluating close reasons (e.g. `CONNECTION_REFUSED` from err 111).
*   `HostnameLoader`:
    *   `Load()`: Loads `hostname_capture.bpf.o`, enumerates all network interfaces (excluding docker/loopback), and attaches BPF filters via Traffic Control (TC) ingress/egress hooks.
*   `CaptureRuntime`:
    *   Wraps the `BPFLoader` to expose a clean start/stop threading surface for the application.

### Process (`src/process`)
*   `ProcessManager` (Implements `IContextProcessor`):
    *   `Process(ProcessingContext&)`: Invoked per network event. Calls `Resolve(pid)` to enrich the context with `ProcessInfo`.
    *   `Resolve(pid)`: Caches processes. On cache miss, reads `/proc/[pid]/stat`, `/proc/[pid]/comm`, and `/proc/[pid]/exe` to extract process names and execution paths. Invokes `TrackPid` on the BPF tracker to monitor the process for exit events.
*   `SocketResolver`:
    *   `GetSockets()`: Scans `/proc/[pid]/fd`, resolving symlinks to extract `socket:[inode]` identifiers.

### Processing (`src/processing`)
*   `ProcessingPipeline`:
    *   `Process()`: Runs `NetworkEvent` sequentially through registered `IContextProcessor`s (like `ProcessManager`, `FlowManager`).
*   `NetworkCoordinator`:
    *   `Coordinate()`: Examines the `ProcessingContext` after pipeline execution. Detects flow/process state changes and publishes `FlowCreatedEvent`, `FlowUpdatedEvent`, `FlowClosedEvent`, or `ProcessClosedEvent` to the Event Bus.
*   `PacketDispatcher`:
    *   `Run()`: Infinitely loops, popping events off the `EventQueue` and feeding them to `ProcessingPipeline::Process()`.

### Presentation (`src/presentation`)
*   `ProcessAggregator`:
    *   `Aggregate()`: Consolidates a flat list of `FlowSnapshot`s, grouping them by PID, summing up active/idle counts and byte transfers, resulting in `ProcessView` models.
*   `ConsoleReporter`:
    *   `Report()`: Triggers aggregation, clears the console using ANSI escape codes, and prints a formatted tabular dashboard of processes and bandwidth usage.

### Storage (`src/storage`)
*   `SQLiteDatabase` / `Statement`:
    *   Wrappers around `sqlite3_*` C-APIs (`sqlite3_open`, `sqlite3_prepare_v2`, `sqlite3_step`). Ensure deterministic cleanup and error formatting.
*   `PersistenceService`:
    *   `HandleEvent()`: Subscribes to Domain Events, queueing them in memory.
    *   `Run()`: Dequeues batches of events. Calls `Optimize()` to collapse redundant updates (e.g., if a flow is CREATED and UPDATED in the same batch, it merges them). Wraps SQLite statements in a transaction (`m_db.BeginTransaction()`) to flush batched `Upsert()` operations.
    *   `RunRetention()`: Background worker executing `DELETE` statements on old closed flows, rolling up hourly bandwidth into daily tables, and clearing orphaned processes.
*   Repositories (`FlowRepository`, `ProcessRepository`, `BandwidthRepository`):
    *   Provides high-level methods like `Upsert()`, internally constructing and executing SQL `INSERT ... ON CONFLICT DO UPDATE` queries binding C++ domain objects to SQLite parameters.
*   `SchemaManager`:
    *   `Initialize()`: Applies pragmas (`WAL` mode, `foreign_keys=ON`) and provisions tables/indices on application startup if they do not exist.

### Application (`src/application`)
*   `NetscopeApplication`:
    *   `Start()`: Initializes the database schema. Bootstraps all modules (Loaders, Dispatchers, Event Busses). Spawns dedicated `std::thread`s for Capture, Dispatcher, Persistence, Retention, Reporter, and RDNS.
    *   `Stop()`: Gracefully signals threads to terminate, flushes queues, unloads BPF programs, and `join()`s all threads to guarantee clean shutdown.

## 3. Core Event & Data Pipeline

1. **Kernel to User-Space (BPF Pipeline):**
   * Packet, socket state, and process exit events are hooked in kernel via eBPF (`packet_monitor.bpf.c` / `hostname_capture.bpf.c`).
   * Events are emitted as raw `bpf_network_event` / `bpf_hostname_event` structs into a BPF Ring Buffer.
   * `BPFLoader::Poll()` and `HostnameLoader::Poll()` read the buffers, convert raw structs into normalized `NetworkEvent` instances, and push them to an thread-safe `EventQueue<NetworkEvent>`.

2. **Real-time Pipeline (Dispatcher -> Pipeline):**
   * `PacketDispatcher` continually dequeues `NetworkEvent` instances.
   * Wraps each event in a `ProcessingContext` and pushes it through the `ProcessingPipeline`.
   * Pipeline stages (`ProcessManager`, `FlowManager`) evaluate and update internal states. `ProcessManager` parses `/proc` to identify unknown PIDs on the fly. `FlowManager` resolves or updates `Flow`s in the `FlowTable`.

3. **Event Generation & Persistence (Coordinator -> Bus -> Storage):**
   * The `ProcessingPipeline` passes the mutated `ProcessingContext` to the `NetworkCoordinator`.
   * The `NetworkCoordinator` analyzes contextual diffs (e.g. `context.flow` changes) and publishes strongly-typed domain events (e.g., `FlowCreatedEvent`, `FlowClosedEvent`) to the `DomainEventBus`.
   * `PersistenceService` receives these events asynchronously, buffers them, optimizes redundant operations, and performs batched transaction writes into the SQLite database.

## 4. Storage & Persistence Layout

The underlying database uses SQLite (WAL mode enabled) and is initialized by `SchemaManager`.

### Schema Map
* **`schema_version`**: `version` (PK), `description`, `applied_at`
* **`app_state`**: `key` (PK), `value`
* **`processes`**: `process_id` (PK), `pid`, `start_time`, `end_time`, `name`, `exe_path`
  * *Constraint*: `UNIQUE(pid, start_time)`
  * *Index*: `idx_processes_pid`
* **`flows`**: `flow_id` (PK), `process_id` (FK), `protocol`, `local_address`, `local_port`, `remote_address`, `remote_port`, `remote_hostname`, `upload_bytes`, `download_bytes`, `upload_packets`, `download_packets`, `first_seen`, `last_seen`, `state`, `close_reason`
  * *Constraint*: `UNIQUE(process_id, protocol, local_address, local_port, remote_address, remote_port)`
  * *Indices*: `idx_flows_active`, `idx_flows_process_id`, `idx_flows_last_seen`, `idx_flows_closed_time`
* **`bandwidth_hourly`**: `id` (PK), `process_id` (FK), `hour_timestamp`, `upload_bytes`, `download_bytes`, `flow_count`
  * *Constraint*: `UNIQUE(process_id, hour_timestamp)`
  * *Index*: `idx_bandwidth_hourly_time`
* **`bandwidth_daily`**: `id` (PK), `process_id` (FK), `day_timestamp`, `upload_bytes`, `download_bytes`, `flow_count`
  * *Constraint*: `UNIQUE(process_id, day_timestamp)`
  * *Index*: `idx_bandwidth_daily_time`

### Repository Contracts
* `AppStateRepository`: Simple KV store interface for application state tracking.
* `BandwidthRepository`: Manages time-series rollups. Supports accumulating hourly stats, rolling them up to daily stats, and purging historical windows.
* `IFlowRepository`: Handles synchronization of `FlowSnapshot`s to the `flows` table.
* `IProcessRepository`: Handles deduplication and insertion of `ProcessInfo`, returning the primary surrogate `process_id` key for relation mapping.

## 5. Boundary Invariants & Rules

To maintain strict, unidirectional architecture and clean modularity, the following cross-layer inclusions are **strictly forbidden**:

1. **`core`** MUST NOT import any module outside of itself. It is the absolute base layer.
2. **`packet_capture`** CANNOT import `storage`, `presentation`, `processing`, or `process`.
3. **`process`** CANNOT import `processing`, `presentation`, or `storage`.
4. **`flow`** CANNOT import `processing`, `presentation`, or `storage`. *(Note: Although CMake currently allows flow to import processing due to legacy binding via IContextProcessor, true domain isolation dictates the flow domain model should not depend on the processing orchestrator)*.
5. **`presentation`** CANNOT import `processing` or `storage`.
6. **`storage`** CANNOT import `processing`, `packet_capture`, or `presentation`.
7. **`hostname`** CANNOT import `processing`, `presentation`, `storage`, or `packet_capture`.

All boundary-crossing communication must occur via domain events (via `core/event`) or high-level orchestration within the `application` layer.
