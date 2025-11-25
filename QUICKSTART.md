# Warehouse OS Simulator - Quick Start Guide

## 🚀 Getting Started

### Build the Project
```bash
make clean-all
make
```

### Run the Simulator
```bash
make run
# or
./warehouse_simulator
```

## 📋 Project Files Overview

| File | Purpose |
|------|---------|
| `Makefile` | Build configuration with pthread support |
| `README.md` | Comprehensive documentation |
| `src/main.cpp` | Entry point (calls WarehouseSimulator) |
| `src/utilities.h` | Common structures, validators, UI helpers |
| `src/data_generator.h` | Random data generation & persistence |
| `src/cpu_scheduler.h` | 5 CPU scheduling algorithms |
| `src/memory_manager.h` | 4 memory allocation strategies |
| `src/disk_scheduler.h` | 4 disk scheduling algorithms |
| `src/synchronization.h` | Producer-Consumer with threads |
| `src/warehouse_simulator.h` | Main controller & menu system |

## 🎯 Key Features Implemented

### ✅ Worker Task Management (CPU Scheduling)
- **FCFS**: First Come First Serve (convoy effect possible)
- **SJF**: Shortest Job First (optimal average waiting time)
- **SRJF**: Shortest Remaining Job First (preemptive SJF)
- **Priority**: Both non-preemptive and preemptive modes
  - Higher number = Higher priority
- **Round Robin**: Configurable time quantum
- **Metrics**: Throughput, CPU Utilization, Avg Waiting/Turnaround Time
- **Output**: ASCII Gantt charts in CSV

### ✅ Storage Rack Allocation (Memory Management)
- **First Fit**: Allocate in first available rack
- **Best Fit**: Find tightest fitting rack (minimal waste)
- **Next Fit**: Continue from last allocation point
- **Worst Fit**: Use largest available rack
- **Features**: 
  - Simultaneous allocation of all processes
  - "Not Allocated" marking for failed allocations
  - Space utilization statistics

### ✅ Truck Movement Management (Disk Scheduling)
- **FCFS**: Process requests in arrival order
- **SSTF**: Serve nearest dock first
- **SCAN**: Elevator-style movement (configurable direction)
- **C-SCAN**: Circular SCAN (uniform wait times)
- **Metrics**: Head sequence, total seek time, average seek time

### ✅ Stock Replenishment (Producer-Consumer)
- **Thread-safe synchronization** using mutexes and condition variables
- **Configurable**: Number of producer and consumer threads
- **Fixed production**: Each producer produces 1 item
- **Fixed consumption**: Each consumer consumes 1 item
- **Buffer management**: Prevents overflow/underflow
- **Output**: Real-time producer/consumer activity logs

### ✅ Data Persistence
- **First run**: User provides ranges, random data generated
- **Subsequent runs**: Option to load previously saved data
- **File**: `data/warehouse_data.txt`
- **On exit**: All data cleared automatically

### ✅ Input Validation
- Negative number rejection
- Non-numeric input handling
- Range validation
- Choice menu validation
- Graceful error recovery

## 📊 Warehouse Analogy Mapping

| OS Concept | Warehouse Component |
|-----------|-------------------|
| Process | Delivery Order |
| CPU | Worker/Employee |
| Memory | Storage Rack/Shelf |
| Disk | Truck/External Storage |
| Buffer | Temporary Stock Area |
| Burst Time | Order Completion Time |
| Arrival Time | Order Entry Time |
| Priority | Delivery Urgency (Express/Normal) |
| I/O Request | Truck Dispatch Request |

## 🔧 Data Generation Parameters

When starting fresh, you'll be prompted for:

**Worker & Task Configuration:**
- Number of orders (max 500)
- Max order completion time
- Max priority level (1-10)

**Storage & Rack Configuration:**
- Total warehouse rack space
- Max individual rack size
- Max order/process size

**Truck & Dispatch Configuration:**
- Number of truck requests
- Max truck capacity (disk size)
- Initial forklift position

**Buffer & Synchronization Configuration:**
- Stock buffer capacity

## 📁 Output Files

Generated CSV files in `output/` directory:

**CPU Scheduling:**
- `fcfs_cpu_results.csv`
- `sjf_cpu_results.csv`
- `srjf_cpu_results.csv`
- `priority_preemptive_cpu_results.csv`
- `priority_non_preemptive_cpu_results.csv`
- `rr_cpu_results.csv`

**Disk Scheduling:**
- `fcfs_disk_results.csv`
- `sstf_disk_results.csv`
- `scan_disk_results.csv`
- `cscan_disk_results.csv`

All CSV files include:
- Process/Request details
- ASCII Gantt charts
- Performance metrics

## 🎮 Interactive Menu Flow

```
┌─ WAREHOUSE SIMULATOR ─────────────────┐
│ 1. Worker Task Management             │
│    ├─ A. FCFS                         │
│    ├─ B. SJF                          │
│    ├─ C. SRJF                         │
│    ├─ D. Priority (Select Mode)       │
│    └─ E. Round Robin (Input TQ)       │
│                                       │
│ 2. Storage Rack Allocation            │
│    ├─ A. First Fit                    │
│    ├─ B. Best Fit                     │
│    ├─ C. Next Fit                     │
│    └─ D. Worst Fit                    │
│                                       │
│ 3. Truck Movement Management          │
│    ├─ A. FCFS                         │
│    ├─ B. SSTF                         │
│    ├─ C. SCAN (Select Direction)      │
│    └─ D. C-SCAN (Select Direction)    │
│                                       │
│ 4. Stock Replenishment                │
│    └─ Producer-Consumer (Threaded)    │
│                                       │
│ 5. Exit (Clears All Data)             │
└───────────────────────────────────────┘
```

## 💡 Usage Tips

1. **First Time**: Choose to generate new data (don't load)
2. **Testing Algorithms**: Load existing data to compare different algorithms
3. **CSV Analysis**: Open generated CSV files in Excel/LibreOffice for charts
4. **Thread Safety**: Producer-Consumer uses proper mutex synchronization
5. **Performance**: For large process counts (>200), SSTF disk scheduling may take longer

## 🐛 Troubleshooting

**Compilation Error with g++:**
```bash
g++ -std=c++17 -pthread -O2 -o warehouse_simulator src/main.cpp
```

**Permission Denied on Linux/Mac:**
```bash
chmod +x warehouse_simulator
./warehouse_simulator
```

**Directory Creation Failed:**
- Ensure write permissions in current directory
- Try running from a different directory

**CSV Files Not Created:**
- Check `output/` directory exists
- Ensure write permissions
- Check disk space

## 📈 Example Workflow

1. **Run**: `make run`
2. **Generate Data**: Enter parameters (e.g., 20 processes, 100 max burst)
3. **Test FCFS**: Navigate to Worker Task Management → FCFS
4. **View Results**: Check console output and `output/fcfs_cpu_results.csv`
5. **Test SJF**: Load existing data → Worker Task Management → SJF
6. **Compare**: Both CSV files show different scheduling effects
7. **Memory Test**: Storage Rack Allocation → Try First Fit and Best Fit
8. **Disk Test**: Truck Movement → Compare FCFS vs SCAN algorithms
9. **Threading**: Stock Coordination → Set 3 producers, 2 consumers
10. **Exit**: All data auto-cleaned

## 🎓 Learning Outcomes

After using this simulator, you'll understand:
- How CPU schedulers improve system performance
- Memory allocation strategies and fragmentation
- Disk I/O optimization techniques
- Thread synchronization with Producer-Consumer
- Performance metrics (utilization, throughput, waiting time)
- Trade-offs between algorithms

## ⚙️ Technical Details

- **Language**: C++17
- **Threading**: `std::thread`, `std::mutex`, `std::condition_variable`
- **Build**: g++ with pthread
- **Platform**: Linux, macOS, Windows (with appropriate build tools)
- **Memory**: In-memory only during session (persistent file storage optional)
- **Complexity**: Clean, modular design with minimal comments

---

**Happy Simulating! 🏭**
