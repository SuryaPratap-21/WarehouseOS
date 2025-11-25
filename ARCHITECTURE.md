# Complete System Architecture & Integration Guide

## 🏗️ Project Architecture Overview

```
warehouse-os-simulator/
│
├── [Makefile] ─────────────────────────── Build Configuration
│   └─ g++ -std=c++17 -pthread
│   └─ Targets: all, run, clean, clean-all, clean-data
│
├── [README.md] ───────────────────────── Comprehensive Docs
├── [QUICKSTART.md] ────────────────────── Quick Reference
├── [IMPLEMENTATION.md] ────────────────── Architecture & Status
│
├── src/ ───────────────────────────────── Source Code
│   │
│   ├── [main.cpp] ────────────────────── Entry Point
│   │   └─ Creates WarehouseSimulator and calls run()
│   │
│   ├── [utilities.h] ─────────────────── Foundation Layer
│   │   ├─ struct Process { id, arrival_time, burst_time, priority, completion_time, waiting_time, turnaround_time }
│   │   ├─ struct MemoryBlock { block_id, size, is_allocated, process_id }
│   │   ├─ struct DiskRequest { request_id, cylinder, arrival_time }
│   │   ├─ class InputValidator (with getPositiveInteger, getChoice, getYesNo)
│   │   ├─ class FileManager (writeToCSV, clearDirectory, createDirectoryIfNeeded)
│   │   └─ class ConsoleUI (printHeader, printSection, printSuccess, etc.)
│   │
│   ├── [data_generator.h] ────────────── Data Management Layer
│   │   ├─ class WarehouseDataGenerator
│   │   ├─ Methods: generateAllData(), generateProcesses(), generateMemoryBlocks(), generateDiskRequests()
│   │   ├─ Data Persistence: saveToFile(), loadFromFile()
│   │   └─ Parameters: num_processes, max_burst_time, max_priority, warehouse_size, etc.
│   │
│   ├── [cpu_scheduler.h] ────────────── Worker Task Management
│   │   ├─ class CPUScheduler
│   │   ├─ Algorithms: runFCFS(), runSJF(), runSRJF(), runPriorityScheduling(), runRoundRobin()
│   │   ├─ Output: displayResults(), saveToCSV()
│   │   ├─ Metrics: cpu_utilization, total_time
│   │   └─ CSV Features: Gantt charts, all metrics per process
│   │
│   ├── [memory_manager.h] ───────────── Storage Rack Management
│   │   ├─ class MemoryManager
│   │   ├─ Strategies: runFirstFit(), runBestFit(), runNextFit(), runWorstFit()
│   │   ├─ Allocation: Tracks block_id, process_id, allocated status
│   │   ├─ Output: displayAllocation()
│   │   └─ Metrics: total_allocated, total_used, wasted_space, utilization%
│   │
│   ├── [disk_scheduler.h] ───────────── Truck Movement Management
│   │   ├─ class DiskScheduler
│   │   ├─ Algorithms: runFCFS(), runSSTF(), runSCAN(), runCSCAN()
│   │   ├─ Direction: User-configurable (higher/lower)
│   │   ├─ Tracking: head_sequence[], total_seek_time
│   │   ├─ Output: displayResults(), saveToCSV()
│   │   └─ Metrics: Total seek time, Average seek time
│   │
│   ├── [synchronization.h] ──────────── Stock Coordination
│   │   ├─ class StockBuffer
│   │   │  ├─ Thread-safe queue with mutex & condition_variable
│   │   │  ├─ Methods: produce(), consume(), getBufferSize()
│   │   │  └─ Synchronization: std::unique_lock, cv.wait(), cv.notify_all()
│   │   │
│   │   └─ class Synchronization
│   │      ├─ Configurable producer/consumer threads
│   │      ├─ Fixed production: 1 item per producer
│   │      ├─ Fixed consumption: 1 item per consumer
│   │      └─ Output: Real-time activity logging
│   │
│   └── [warehouse_simulator.h] ──────── Main Controller
│       ├─ class WarehouseSimulator
│       ├─ Methods: initialize(), run(), loadData()
│       ├─ Menu System: displayMainMenu()
│       ├─ Workflow:
│       │  ├─ runWorkerTaskManagement()
│       │  ├─ runStorageRackAllocation()
│       │  ├─ runTruckMovementManagement()
│       │  ├─ runStockReplenishment()
│       │  └─ exitSimulation()
│       └─ Data Integration: Connects all modules
│
├── data/ ──────────────────────────────── Generated Data
│   └─ [warehouse_data.txt] ────────── Persistent warehouse state
│       ├─ Parameters: 10 configuration values
│       ├─ Processes: id, arrival_time, burst_time, priority
│       ├─ Memory Blocks: block_id, size
│       └─ Disk Requests: request_id, cylinder, arrival_time
│
└── output/ ────────────────────────────── Simulation Results
    ├─ [fcfs_cpu_results.csv] ──────── CPU scheduling results
    ├─ [sjf_cpu_results.csv]
    ├─ [srjf_cpu_results.csv]
    ├─ [priority_*.csv]
    ├─ [rr_cpu_results.csv]
    ├─ [fcfs_disk_results.csv] ────── Disk scheduling results
    ├─ [sstf_disk_results.csv]
    ├─ [scan_disk_results.csv]
    └─ [cscan_disk_results.csv]
```

---

## 🔄 Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│  APPLICATION START (main.cpp)                               │
└────────────────────┬────────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────────┐
│  WarehouseSimulator::initialize()                           │
│  ├─ Check for saved data (data/warehouse_data.txt)         │
│  ├─ If new: Prompt all 10 parameters                       │
│  └─ If existing: Load from file                            │
└────────────────────┬────────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────────┐
│  WarehouseDataGenerator::generateAllData()                  │
│  ├─ generateProcesses() ──► processes[] (sorted by arrival) │
│  ├─ generateMemoryBlocks() ──► memory_blocks[]             │
│  ├─ generateDiskRequests() ──► disk_requests[]             │
│  └─ saveToFile() ──► data/warehouse_data.txt               │
└────────────────────┬────────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────────┐
│  MAIN MENU LOOP (displayMainMenu())                         │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ User Choice: 1-5                                    │   │
│  └──────────────────────────────────────────────────────┘   │
└────────────┬──────────────┬──────────────┬───────────┬───────┘
             │              │              │           │
      ┌──────▼──┐    ┌──────▼──┐    ┌────▼──┐    ┌───▼───┐
      │ Option  │    │ Option  │    │ Option│    │Option │
      │    1    │    │    2    │    │   3   │    │  4    │
      └──────┬──┘    └──────┬──┘    └────┬──┘    └───┬───┘
             │              │            │          │
      ┌──────▼────────┐  ┌──▼───────────┐ ┌──────▼───┐
      │CPU Scheduler  │  │Memory Manager│ │Disk      │
      │5 Algorithms   │  │4 Strategies  │ │Scheduler │
      │+ Metrics      │  │+ Stats       │ │4 Algos   │
      └──────┬────────┘  └──┬───────────┘ └──────┬───┘
             │              │                    │
      ┌──────▼────────────┬─▼────────┬──────┬────▼──────┐
      │Process Results    │Allocation │Seek  │Sync Logs  │
      │+ Gantt Chart      │Results    │Stats │+ Buffers  │
      │+ CSV Export       │+ CSV      │+ CSV │           │
      └───────────────────┴───────────┴──────┴───────────┘
```

---

## 🎛️ Component Integration Map

```
INPUT VALIDATION (InputValidator)
    ↓
DATA GENERATION (WarehouseDataGenerator)
    ├─ Processes → CPU Scheduler
    ├─ Memory Blocks → Memory Manager
    └─ Disk Requests → Disk Scheduler
    │
    └─ Persistent Storage (FileManager)
        └─ data/warehouse_data.txt

ALGORITHMS
    ├─ CPUScheduler (5 algorithms)
    │   ├─ Shared: Process[], metrics calculation
    │   └─ Output: CSV + Gantt + Console
    │
    ├─ MemoryManager (4 strategies)
    │   ├─ Shared: MemoryBlock[], allocation tracking
    │   └─ Output: Console + Statistics
    │
    ├─ DiskScheduler (4 algorithms)
    │   ├─ Shared: DiskRequest[], head tracking
    │   └─ Output: CSV + Statistics
    │
    └─ Synchronization (Producer-Consumer)
        ├─ Thread Pool: std::thread[]
        ├─ Sync: std::mutex, std::condition_variable
        └─ Output: Real-time console logs

COMMON UTILITIES
    ├─ ConsoleUI: All output formatting
    ├─ FileManager: All file I/O
    ├─ InputValidator: All input handling
    └─ Structures: Process, MemoryBlock, DiskRequest

MAIN CONTROLLER (WarehouseSimulator)
    └─ Orchestrates all components
```

---

## 🔐 Thread Safety Strategy

```
Producer-Consumer Synchronization:

┌─────────────────────────────────────┐
│  StockBuffer (Thread-Safe)          │
│  ┌─────────────────────────────────┐│
│  │ Private:                        ││
│  │  - std::queue<int> items        ││
│  │  - std::mutex mutex             ││
│  │  - std::condition_variable cv   ││
│  └─────────────────────────────────┘│
└─────────────────────────────────────┘
         ▲                    ▲
         │                    │
    Producer Threads      Consumer Threads
         │                    │
    produce() calls:      consume() calls:
    ├─ cv.wait([...]) ──────┤ cv.wait([...])
    ├─ Acquire lock         │ Acquire lock
    ├─ Add to queue         │ Remove from queue
    ├─ cv.notify_all()      │ cv.notify_all()
    └─ Release lock         └─ Release lock
```

---

## 📊 CSV Output Structure

### CPU Scheduling CSV:
```
Order_ID,Arrival_Time,Burst_Time,Priority,Completion_Time,Waiting_Time,Turnaround_Time
1,5,10,5,15,0,10
2,2,8,3,23,13,21
3,8,6,7,14,0,6
...
Gantt Chart: |P1|P3|P2|...
```

### Memory Allocation CSV:
```
Order_ID,Order_Size,Rack_Number
P1,50,B1
P2,75,B2
P3,40,Not Allocated
...
```

### Disk Scheduling CSV:
```
Truck_Request_ID,Dock_Cylinder,Arrival_Time
1,45,0
2,123,5
3,23,10
...
Head Sequence: 50 → 45 → 23 → 123
Total Seek Time,296
Avg Seek Time,98.67
```

---

## 🎯 Algorithm Complexity Analysis

### CPU Scheduling:
- **FCFS**: O(n log n) - Sort + Linear processing
- **SJF**: O(n²) - Selection for each time unit
- **SRJF**: O(n²) - Preemptive selection
- **Priority**: O(n²) - Priority queue operations
- **Round Robin**: O(n × time_quantum)

### Memory Allocation:
- **First Fit**: O(n) - Single pass
- **Best Fit**: O(n) - Find minimum waste
- **Next Fit**: O(n) - Circular search
- **Worst Fit**: O(n) - Find maximum

### Disk Scheduling:
- **FCFS**: O(n log n)
- **SSTF**: O(n²) - Nearest neighbor
- **SCAN**: O(n log n) - Sorted traversal
- **C-SCAN**: O(n log n) - Sorted traversal

---

## 🚀 Execution Flow Example

```
1. User runs: ./warehouse_simulator

2. WarehouseSimulator::initialize()
   └─ Check for saved data
   └─ If yes: Load from file
   └─ If no: Prompt 10 parameters
   └─ Generate random data
   └─ Save to data/warehouse_data.txt

3. Display Main Menu
   User selects: 1 (Worker Task Management)

4. CPU Scheduler Menu
   User selects: B (SJF)

5. CPUScheduler::runSJF()
   ├─ Sort by burst time
   ├─ Calculate completion/waiting/turnaround
   ├─ Calculate metrics
   ├─ Display results on console
   └─ Save to output/sjf_cpu_results.csv

6. Return to Main Menu
   User selects: 2 (Storage Rack Allocation)

7. Memory Manager Menu
   User selects: A (First Fit)

8. MemoryManager::runFirstFit()
   ├─ Iterate through processes
   ├─ Find first available block
   ├─ Mark allocated
   ├─ Display allocation table
   └─ Show statistics

9. Loop continues until user selects option 5 (Exit)

10. WarehouseSimulator::exitSimulation()
    ├─ Clear all data files
    ├─ Clear all output files
    └─ Exit cleanly
```

---

## ✅ Quality Assurance Checklist

- [x] All 5 CPU scheduling algorithms implemented
- [x] All 4 memory allocation strategies implemented
- [x] All 4 disk scheduling algorithms implemented
- [x] Producer-Consumer with proper thread synchronization
- [x] Data persistence (save/load)
- [x] Input validation with error handling
- [x] CSV export with Gantt charts
- [x] Cross-platform build (g++, pthread)
- [x] Modular architecture
- [x] Warehouse analogy mapping
- [x] Comprehensive documentation
- [x] Makefile with multiple targets
- [x] README and Quick Start guides

---

## 🎓 Learning Path

**Understanding the Code:**
1. Start with `utilities.h` - Basic structures
2. Read `warehouse_simulator.h` - Overall flow
3. Study `data_generator.h` - Data creation
4. Explore `cpu_scheduler.h` - First algorithm
5. Compare other algorithm implementations
6. Understand `synchronization.h` - Threading

**Running Experiments:**
1. Small dataset (5-10 processes)
2. Compare FCFS vs SJF on same data
3. Test memory allocation with limited space
4. Run disk scheduling with different head positions
5. Producer-Consumer with varying thread counts

**Extending the Project:**
1. Add new CPU algorithm (MLQ, Feedback)
2. Implement memory compaction
3. Add statistics comparison tool
4. Create graphical visualization
5. Add configuration file support

---

**This completes the comprehensive Warehouse OS Simulator project!** 🎉
