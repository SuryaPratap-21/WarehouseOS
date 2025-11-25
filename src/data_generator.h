#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include "utilities.h"
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>

class WarehouseDataGenerator {
private:
    std::vector<Process> processes;
    std::vector<MemoryBlock> memory_blocks;
    std::vector<DiskRequest> disk_requests;
    int num_processes;
    int max_burst_time;
    int max_priority;
    int warehouse_size;
    int max_block_size;
    int max_process_size;
    int num_disk_requests;
    int max_disk_size;
    int head_position;
    int buffer_size;

    unsigned seed;

public:
    WarehouseDataGenerator() : seed(std::chrono::system_clock::now().time_since_epoch().count()) {}

    void generateAllData(bool use_saved_data = false) {
        if (use_saved_data && loadFromFile()) {
            ConsoleUI::printSuccess("Loaded previously saved warehouse data");
            return;
        }

        ConsoleUI::printHeader("WAREHOUSE DATA GENERATION");
        ConsoleUI::printInfo("Configuring warehouse parameters (warehouse analogy)");

        std::cout << "\n📦 WORKER & TASK CONFIGURATION:\n";
        num_processes = InputValidator::getPositiveInteger("  Number of orders (processes, max 500): ", 500);
        max_burst_time = InputValidator::getPositiveInteger("  Max order completion time (max burst): ");
        max_priority = InputValidator::getPositiveInteger("  Max priority level (1-10): ", 10);

        std::cout << "\n🏢 STORAGE & RACK CONFIGURATION:\n";
        warehouse_size = InputValidator::getPositiveInteger("  Total warehouse rack space (size): ");
        max_block_size = InputValidator::getPositiveInteger("  Max individual rack size: ");
        max_process_size = InputValidator::getPositiveInteger("  Max order/process size: ");

        std::cout << "\n🚚 TRUCK & DISPATCH CONFIGURATION:\n";
        num_disk_requests = InputValidator::getPositiveInteger("  Number of truck requests: ");
        max_disk_size = InputValidator::getPositiveInteger("  Max disk/truck capacity: ");
        head_position = InputValidator::getPositiveInteger("  Initial forklift position (dock): ");

        std::cout << "\n📊 BUFFER & SYNCHRONIZATION CONFIGURATION:\n";
        buffer_size = InputValidator::getPositiveInteger("  Stock buffer capacity: ");

        generateProcesses();
        generateMemoryBlocks();
        generateDiskRequests();
        saveToFile();
        printStatistics();
    }

private:
    void generateProcesses() {
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> arrival_dist(0, max_burst_time * 2);
        std::uniform_int_distribution<> burst_dist(1, max_burst_time);
        std::uniform_int_distribution<> priority_dist(1, max_priority);

        processes.clear();
        for (int i = 0; i < num_processes; ++i) {
            Process p;
            p.id = i + 1;
            p.arrival_time = arrival_dist(gen);
            p.burst_time = burst_dist(gen);
            p.priority = priority_dist(gen);
            p.completion_time = 0;
            p.waiting_time = 0;
            p.turnaround_time = 0;
            processes.push_back(p);
        }
        std::sort(processes.begin(), processes.end(),
                 [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });
    }

    void generateMemoryBlocks() {
        std::mt19937 gen(seed + 1);
        std::uniform_int_distribution<> block_size_dist(100, max_block_size);

        memory_blocks.clear();
        int current_offset = 0;
        int block_id = 1;

        while (current_offset < warehouse_size) {
            MemoryBlock block;
            block.block_id = block_id++;
            block.size = std::min(block_size_dist(gen), warehouse_size - current_offset);
            block.is_allocated = false;
            block.process_id = -1;
            memory_blocks.push_back(block);
            current_offset += block.size;
        }
    }

    void generateDiskRequests() {
        std::mt19937 gen(seed + 2);
        std::uniform_int_distribution<> cylinder_dist(0, max_disk_size);
        std::uniform_int_distribution<> arrival_dist(0, num_processes * max_burst_time / 2);

        disk_requests.clear();
        for (int i = 0; i < num_disk_requests; ++i) {
            DiskRequest req;
            req.request_id = i + 1;
            req.cylinder = cylinder_dist(gen);
            req.arrival_time = arrival_dist(gen);
            disk_requests.push_back(req);
        }
    }

    void saveToFile() {
        std::ofstream file("data/warehouse_data.txt");
        if (!file.is_open()) {
            std::cerr << "❌ Could not create data file" << std::endl;
            return;
        }

        file << num_processes << "\n" << max_burst_time << "\n" << max_priority << "\n"
             << warehouse_size << "\n" << max_block_size << "\n" << max_process_size << "\n"
             << num_disk_requests << "\n" << max_disk_size << "\n" << head_position << "\n"
             << buffer_size << "\n";

        for (const auto& p : processes) {
            file << p.id << "," << p.arrival_time << "," << p.burst_time << "," << p.priority << "\n";
        }
        file << "---\n";

        for (const auto& b : memory_blocks) {
            file << b.block_id << "," << b.size << "\n";
        }
        file << "---\n";

        for (const auto& d : disk_requests) {
            file << d.request_id << "," << d.cylinder << "," << d.arrival_time << "\n";
        }

        file.close();
    }

    bool loadFromFile() {
        std::ifstream file("data/warehouse_data.txt");
        if (!file.is_open()) return false;

        try {
            file >> num_processes >> max_burst_time >> max_priority >> warehouse_size
                 >> max_block_size >> max_process_size >> num_disk_requests >> max_disk_size
                 >> head_position >> buffer_size;

            processes.clear();
            std::string line;
            std::getline(file, line);

            while (std::getline(file, line)) {
                if (line == "---") break;
                std::istringstream iss(line);
                Process p;
                char comma;
                iss >> p.id >> comma >> p.arrival_time >> comma >> p.burst_time >> comma >> p.priority;
                p.completion_time = p.waiting_time = p.turnaround_time = 0;
                processes.push_back(p);
            }

            memory_blocks.clear();
            while (std::getline(file, line)) {
                if (line == "---") break;
                std::istringstream iss(line);
                MemoryBlock b;
                char comma;
                iss >> b.block_id >> comma >> b.size;
                b.is_allocated = false;
                b.process_id = -1;
                memory_blocks.push_back(b);
            }

            disk_requests.clear();
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                DiskRequest d;
                char comma;
                iss >> d.request_id >> comma >> d.cylinder >> comma >> d.arrival_time;
                disk_requests.push_back(d);
            }

            file.close();
            return true;
        } catch (...) {
            return false;
        }
    }

    void printStatistics() {
        ConsoleUI::printSection("GENERATED DATA STATISTICS");
        std::cout << "✓ Processes: " << num_processes << std::endl;
        std::cout << "✓ Memory Blocks: " << memory_blocks.size() << std::endl;
        std::cout << "✓ Disk Requests: " << num_disk_requests << std::endl;
        std::cout << "✓ Warehouse Capacity: " << warehouse_size << " units\n";
        std::cout << "✓ Buffer Capacity: " << buffer_size << " units\n";
    }

public:
    const std::vector<Process>& getProcesses() const { return processes; }
    const std::vector<MemoryBlock>& getMemoryBlocks() const { return memory_blocks; }
    const std::vector<DiskRequest>& getDiskRequests() const { return disk_requests; }
    int getWarehouseSize() const { return warehouse_size; }
    int getMaxProcessSize() const { return max_process_size; }
    int getMaxDiskSize() const { return max_disk_size; }
    int getHeadPosition() const { return head_position; }
    int getBufferSize() const { return buffer_size; }
};

#endif
