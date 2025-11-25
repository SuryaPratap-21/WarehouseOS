# Warehouse Operating System Simulator

A comprehensive C++17 simulation of operating system concepts using warehouse management as an analogy.

## Project Overview

This project implements five core OS concepts through a warehouse management system:

1. **Worker Task Management (CPU Scheduling)**
   - FCFS, SJF, SRJF, Priority, Round Robin scheduling algorithms
   - Metrics: Throughput, CPU Utilization, Avg Waiting Time, Avg Turnaround Time

2. **Storage Rack Allocation (Memory Management)**
   - First Fit, Best Fit, Next Fit, Worst Fit allocation strategies
   - Tracks allocation efficiency and fragmentation

3. **Truck Movement Management (Disk Scheduling)**
   - FCFS, SSTF, SCAN, C-SCAN disk scheduling algorithms
   - Calculates seek sequences and total seek time

4. **Stock Replenishment Coordination (Synchronization)**
   - Producer-Consumer problem with multithreading
   - Thread-safe queue synchronization

5. **Complete System Simulation**
   - Persistent data generation and reuse
   - CSV output with Gantt charts
   - Interactive menu-driven interface

## Requirements

- **C++17** or later
- **g++** compiler with pthread support
- **Make** utility
- Linux/macOS/Windows with standard C++ library

## Building & Running

### Compile the project:
```bash
make clean-all
make
```

### Run the simulator:
```bash
make run
```

Or directly:
```bash
./warehouse_simulator
```

### Clean up:
```bash
make clean           # Remove executable only
make clean-data      # Remove generated data and outputs
make clean-all       # Full cleanup
```

## Project Structure

```
warehouse-os-simulator/
├── Makefile                 # Build configuration
├── README.md               # This file
├── src/
│   ├── main.cpp           # Entry point and main menu
│   ├── data_generator.h   # Random data generation
│   ├── utilities.h        # Common utilities and structures
│   ├── cpu_scheduler.h    # CPU/Worker scheduling algorithms
│   ├── memory_manager.h   # Memory/Storage allocation algorithms
│   ├── disk_scheduler.h   # Disk/Truck scheduling algorithms
│   ├── synchronization.h  # Producer-Consumer synchronization
│   └── warehouse_simulator.h # Main simulation controller
├── data/
│   └── warehouse_data.txt  # Generated and reused warehouse data
└── output/
    └── *.csv              # Generated results and Gantt charts
```

## Usage Guide

### Initial Setup
1. Run the program: `make run`
2. Input simulation parameters (process count, burst time, warehouse size, etc.)
3. Data is automatically saved to `data/warehouse_data.txt`

### Main Menu Options

**1. Worker Task Management**
   - Select scheduling algorithm (FCFS, SJF, SRJF, Priority, Round Robin)
   - For Priority: Choose preemptive or non-preemptive
   - For Round Robin: Enter time quantum
   - Output: CSV file with process details and Gantt chart

**2. Storage Rack Allocation**
   - Select allocation strategy (First Fit, Best Fit, Next Fit, Worst Fit)
   - Displays allocation table and utilization metrics

**3. Truck Movement Management**
   - Select scheduling algorithm (FCFS, SSTF, SCAN, C-SCAN)
   - For SCAN/C-SCAN: Specify initial direction
   - Output: CSV with head sequence and seek calculations

**4. Stock Replenishment Coordination**
   - Enter number of producer threads
   - Enter number of consumer threads
   - Simulation runs with thread synchronization
   - Displays producer-consumer interactions

**5. Exit**
   - Clears all generated data
   - Exits simulation

## Output Files

CSV files are generated in the `output/` directory:

- `fcfs_cpu_results.csv` - FCFS scheduling results
- `sjf_cpu_results.csv` - SJF scheduling results
- `srjf_cpu_results.csv` - SRJF scheduling results
- `priority_cpu_results.csv` - Priority scheduling results
- `rr_cpu_results.csv` - Round Robin results
- Memory allocation results (per algorithm)
- Disk scheduling results (per algorithm)

Each CSV includes process/request details and ASCII Gantt chart representation.

## Key Features

✅ **Modular Design** - Clean separation of concerns with header files
✅ **Input Validation** - Comprehensive error handling and input validation
✅ **Data Persistence** - Generate once, reuse across simulations
✅ **Multithreading** - Thread-safe synchronization for producer-consumer
✅ **ASCII Visualizations** - Gantt charts in CSV format
✅ **Comprehensive Metrics** - Detailed performance statistics
✅ **Cross-Platform** - Compatible with Linux, macOS, Windows
✅ **C++17 Standard** - Modern C++ practices and features

## Example Workflow

```
1. Start simulation
2. Enter parameters (e.g., 10 processes, max burst 100, warehouse size 1000)
3. Select "Worker Task Management"
4. Choose "B. Shortest Job First (SJF)"
5. View results in console
6. Check output/sjf_cpu_results.csv for detailed data
7. Return to main menu or try another algorithm
8. Select "Exit" when done
```

## Warehouse Analogy

| OS Concept | Warehouse Component |
|-----------|-------------------|
| Process | Delivery Order |
| CPU | Worker |
| Memory | Storage Rack |
| Disk | Truck/External Storage |
| Burst Time | Order Completion Time |
| I/O Request | Truck Movement Request |
| Buffer | Temporary Stock Area |
| Context Switch | Worker Task Switching |

## Performance Metrics

### CPU Scheduling:
- **Throughput** - Orders processed per time unit
- **CPU Utilization** - Percentage of worker busy time
- **Waiting Time** - Average order wait before processing
- **Turnaround Time** - Average total time from arrival to completion

### Disk Scheduling:
- **Total Seek Time** - Sum of all forklift movements
- **Head Movement Sequence** - Order of dock visits
- **Average Seek Time** - Mean distance per movement