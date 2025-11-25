#ifndef WAREHOUSE_SIMULATOR_H
#define WAREHOUSE_SIMULATOR_H

#include "data_generator.h"
#include "cpu_scheduler.h"
#include "memory_manager.h"
#include "disk_scheduler.h"
#include "synchronization.h"

class WarehouseSimulator {
private:
    WarehouseDataGenerator data_gen;
    std::vector<Process> processes;
    std::vector<MemoryBlock> memory_blocks;
    std::vector<DiskRequest> disk_requests;
    int warehouse_size;
    int max_process_size;
    int max_disk_size;
    int head_position;
    int buffer_size;

public:
    WarehouseSimulator() : warehouse_size(0), max_process_size(0), max_disk_size(0), head_position(0), buffer_size(0) {
        FileManager::createDirectoryIfNeeded("data");
        FileManager::createDirectoryIfNeeded("output");
    }

    void initialize() {
        ConsoleUI::printHeader("WAREHOUSE OPERATING SYSTEM SIMULATOR");
        ConsoleUI::printInfo("Initialize warehouse with data generation or load existing data\n");

        bool use_existing = false;
        char choice;
        std::cout << "Do you want to load previously saved data? (y/n): ";
        std::cin >> choice;

        if (choice == 'y' || choice == 'Y') {
            use_existing = true;
        }

        data_gen.generateAllData(use_existing);
        loadData();
    }

    void loadData() {
        processes = data_gen.getProcesses();
        memory_blocks = data_gen.getMemoryBlocks();
        disk_requests = data_gen.getDiskRequests();
        warehouse_size = data_gen.getWarehouseSize();
        max_process_size = data_gen.getMaxProcessSize();
        max_disk_size = data_gen.getMaxDiskSize();
        head_position = data_gen.getHeadPosition();
        buffer_size = data_gen.getBufferSize();
    }

    void run() {
        initialize();

        while (true) {
            displayMainMenu();
            int choice = InputValidator::getChoice(1, 5);

            switch (choice) {
                case 1:
                    runWorkerTaskManagement();
                    break;
                case 2:
                    runStorageRackAllocation();
                    break;
                case 3:
                    runTruckMovementManagement();
                    break;
                case 4:
                    runStockReplenishment();
                    break;
                case 5:
                    exitSimulation();
                    return;
                default:
                    ConsoleUI::printError("Invalid choice");
            }
        }
    }

private:
    void displayMainMenu() {
        ConsoleUI::printHeader("WAREHOUSE MANAGEMENT SYSTEM MAIN MENU");
        std::cout << "\n1. 👷 Worker Task Management (CPU Scheduling)\n";
        std::cout << "2. 🏢 Storage Rack Allocation (Memory Management)\n";
        std::cout << "3. 🚚 Truck Movement Management (Disk Scheduling)\n";
        std::cout << "4. 📦 Stock Replenishment Coordination (Synchronization)\n";
        std::cout << "5. 🚪 Exit\n";
    }

    void runWorkerTaskManagement() {
        ConsoleUI::printHeader("WORKER TASK MANAGEMENT");
        std::cout << "\nSelect a scheduling algorithm:\n";
        std::cout << "A. First Come First Serve (FCFS)\n";
        std::cout << "B. Shortest Job First (SJF)\n";
        std::cout << "C. Shortest Remaining Job First (SRJF)\n";
        std::cout << "D. Priority Scheduling\n";
        std::cout << "E. Round Robin (RR)\n";
        std::cout << "F. Back to Main Menu\n";

        char algo_choice;
        std::cout << "Enter choice (A-F): ";
        std::cin >> algo_choice;

        CPUScheduler scheduler(processes);

        switch (algo_choice) {
            case 'A':
            case 'a':
                scheduler.runFCFS();
                break;
            case 'B':
            case 'b':
                scheduler.runSJF();
                break;
            case 'C':
            case 'c':
                scheduler.runSRJF();
                break;
            case 'D':
            case 'd': {
                std::cout << "Choose priority scheduling mode:\n";
                std::cout << "1. Non-Preemptive Priority Scheduling\n";
                std::cout << "2. Preemptive Priority Scheduling\n";
                int priority_choice = InputValidator::getChoice(1, 2);
                scheduler.runPriorityScheduling(priority_choice == 2);
                break;
            }
            case 'E':
            case 'e':
                scheduler.runRoundRobin();
                break;
            case 'F':
            case 'f':
                return;
            default:
                ConsoleUI::printError("Invalid choice");
        }

        std::cout << "\nPress Enter to return to main menu...";
        std::cin.ignore();
        std::cin.get();
    }

    void runStorageRackAllocation() {
        ConsoleUI::printHeader("STORAGE RACK ALLOCATION");
        std::cout << "\nSelect allocation strategy:\n";
        std::cout << "A. First Fit\n";
        std::cout << "B. Best Fit\n";
        std::cout << "C. Next Fit\n";
        std::cout << "D. Worst Fit\n";
        std::cout << "E. Back to Main Menu\n";

        char alloc_choice;
        std::cout << "Enter choice (A-E): ";
        std::cin >> alloc_choice;

        MemoryManager memory_mgr(memory_blocks, processes, warehouse_size, max_process_size);

        switch (alloc_choice) {
            case 'A':
            case 'a':
                memory_mgr.runFirstFit();
                break;
            case 'B':
            case 'b':
                memory_mgr.runBestFit();
                break;
            case 'C':
            case 'c':
                memory_mgr.runNextFit();
                break;
            case 'D':
            case 'd':
                memory_mgr.runWorstFit();
                break;
            case 'E':
            case 'e':
                return;
            default:
                ConsoleUI::printError("Invalid choice");
        }

        std::cout << "\nPress Enter to return to main menu...";
        std::cin.ignore();
        std::cin.get();
    }

    void runTruckMovementManagement() {
        ConsoleUI::printHeader("TRUCK MOVEMENT MANAGEMENT");
        std::cout << "\nSelect disk scheduling algorithm:\n";
        std::cout << "A. First Come First Serve (FCFS)\n";
        std::cout << "B. Shortest Seek Time First (SSTF)\n";
        std::cout << "C. SCAN (Elevator Algorithm)\n";
        std::cout << "D. C-SCAN (Circular SCAN)\n";
        std::cout << "E. Back to Main Menu\n";

        char disk_choice;
        std::cout << "Enter choice (A-E): ";
        std::cin >> disk_choice;

        DiskScheduler disk_sched(disk_requests, head_position, max_disk_size);

        switch (disk_choice) {
            case 'A':
            case 'a':
                disk_sched.runFCFS();
                break;
            case 'B':
            case 'b':
                disk_sched.runSSTF();
                break;
            case 'C':
            case 'c':
                disk_sched.runSCAN();
                break;
            case 'D':
            case 'd':
                disk_sched.runCSCAN();
                break;
            case 'E':
            case 'e':
                return;
            default:
                ConsoleUI::printError("Invalid choice");
        }

        std::cout << "\nPress Enter to return to main menu...";
        std::cin.ignore();
        std::cin.get();
    }

    void runStockReplenishment() {
        ConsoleUI::printHeader("STOCK REPLENISHMENT COORDINATION");

        int num_threads = InputValidator::getPositiveInteger("Enter number of threads: ");

        Synchronization sync(buffer_size, num_threads, num_threads);
        sync.runProducerConsumer();

        std::cout << "\nPress Enter to return to main menu...";
        std::cin.ignore();
        std::cin.get();
    }

    void exitSimulation() {
        ConsoleUI::printHeader("EXITING WAREHOUSE SIMULATOR");
        ConsoleUI::printInfo("Cleaning up generated data...");
        FileManager::clearDirectory("data");
        FileManager::clearDirectory("output");
        ConsoleUI::printSuccess("All data cleared. Goodbye!");
    }
};

#endif
