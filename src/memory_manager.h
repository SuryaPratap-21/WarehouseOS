#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "utilities.h"
#include <algorithm>

class MemoryManager {
private:
    std::vector<MemoryBlock> blocks;
    std::vector<Process> processes;
    int warehouse_size;
    int max_process_size;
    int next_fit_index;

public:
    MemoryManager(const std::vector<MemoryBlock>& mem_blocks, const std::vector<Process>& procs, int size, int max_size)
        : blocks(mem_blocks), processes(procs), warehouse_size(size), max_process_size(max_size), next_fit_index(0) {}

    void runFirstFit() {
        ConsoleUI::printSection("First Fit Allocation - Place in First Available Rack");
        allocateFirstFit();
        displayAllocation("First Fit");
    }

    void runBestFit() {
        ConsoleUI::printSection("Best Fit Allocation - Find Tightest Fitting Rack");
        allocateBestFit();
        displayAllocation("Best Fit");
    }

    void runNextFit() {
        ConsoleUI::printSection("Next Fit Allocation - Continuous Allocation from Last Position");
        allocateNextFit();
        displayAllocation("Next Fit");
    }

    void runWorstFit() {
        ConsoleUI::printSection("Worst Fit Allocation - Place in Largest Available Rack");
        allocateWorstFit();
        displayAllocation("Worst Fit");
    }

private:
    void allocateFirstFit() {
        for (const auto& process : processes) {
            bool allocated = false;
            for (auto& block : blocks) {
                if (!block.is_allocated && block.size >= process.burst_time) {
                    block.is_allocated = true;
                    block.process_id = process.id;
                    allocated = true;
                    break;
                }
            }
        }
    }

    void allocateBestFit() {
        for (const auto& process : processes) {
            int best_block = -1;
            int min_wasted_space = INT_MAX;

            for (size_t i = 0; i < blocks.size(); ++i) {
                if (!blocks[i].is_allocated && blocks[i].size >= process.burst_time) {
                    int wasted = blocks[i].size - process.burst_time;
                    if (wasted < min_wasted_space) {
                        min_wasted_space = wasted;
                        best_block = i;
                    }
                }
            }

            if (best_block != -1) {
                blocks[best_block].is_allocated = true;
                blocks[best_block].process_id = process.id;
            }
        }
    }

    void allocateNextFit() {
        for (const auto& process : processes) {
            for (size_t i = 0; i < blocks.size(); ++i) {
                int index = (next_fit_index + i) % blocks.size();
                if (!blocks[index].is_allocated && blocks[index].size >= process.burst_time) {
                    blocks[index].is_allocated = true;
                    blocks[index].process_id = process.id;
                    next_fit_index = (index + 1) % blocks.size();
                    break;
                }
            }
        }
    }

    void allocateWorstFit() {
        for (const auto& process : processes) {
            int worst_block = -1;
            int max_size = -1;

            for (size_t i = 0; i < blocks.size(); ++i) {
                if (!blocks[i].is_allocated && blocks[i].size >= process.burst_time) {
                    if (blocks[i].size > max_size) {
                        max_size = blocks[i].size;
                        worst_block = i;
                    }
                }
            }

            if (worst_block != -1) {
                blocks[worst_block].is_allocated = true;
                blocks[worst_block].process_id = process.id;
            }
        }
    }

    void displayAllocation(const std::string& algorithm) {
        ConsoleUI::printSection("STORAGE ALLOCATION RESULTS - " + algorithm);

        int total_allocated = 0;
        int total_used = 0;
        int allocated_count = 0;

        std::cout << std::left << std::setw(12) << "Order_ID" << std::setw(12) << "Order_Size" << std::setw(12) << "Rack_Number" << "\n";
        std::cout << std::string(36, '-') << "\n";

        for (const auto& process : processes) {
            bool found = false;
            for (const auto& block : blocks) {
                if (block.process_id == process.id) {
                    std::cout << std::left << std::setw(12) << "P" + std::to_string(process.id)
                              << std::setw(12) << process.burst_time
                              << std::setw(12) << "B" + std::to_string(block.block_id) << "\n";
                    total_allocated += block.size;
                    total_used += process.burst_time;
                    allocated_count++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << std::left << std::setw(12) << "P" + std::to_string(process.id)
                          << std::setw(12) << process.burst_time
                          << std::setw(12) << "Not Allocated" << "\n";
            }
        }

        std::cout << "\n" << std::string(36, '-') << "\n";
        std::cout << "ALLOCATION STATISTICS:\n";
        std::cout << "Successfully Allocated: " << allocated_count << "/" << processes.size() << "\n";
        std::cout << "Total Space Allocated: " << total_allocated << " units\n";
        std::cout << "Total Space Used: " << total_used << " units\n";
        std::cout << "Wasted Space: " << (total_allocated - total_used) << " units\n";

        if (total_allocated > 0) {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Memory Utilization: " << ((total_used * 100.0) / warehouse_size) << "%\n";
        }
    }
};

#endif
