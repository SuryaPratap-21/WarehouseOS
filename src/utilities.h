#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cstdlib>

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int priority;
    int completion_time;
    int waiting_time;
    int turnaround_time;
};

struct MemoryBlock {
    int block_id;
    int size;
    bool is_allocated;
    int process_id;
};

struct DiskRequest {
    int request_id;
    int cylinder;
    int arrival_time;
};

class InputValidator {
public:
    static int getPositiveInteger(const std::string& prompt, int max_value = INT_MAX) {
        int value;
        while (true) {
            std::cout << prompt;
            if (!(std::cin >> value)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "❌ Invalid input. Please enter a valid number.\n";
                continue;
            }
            if (value < 0) {
                std::cout << "❌ Negative values not allowed. Please enter a non-negative number.\n";
                continue;
            }
            if (value > max_value) {
                std::cout << "❌ Value exceeds maximum (" << max_value << "). Please try again.\n";
                continue;
            }
            if (value == 0) {
                std::cout << "⚠️  Zero is not useful for simulation. Please enter a positive number.\n";
                continue;
            }
            return value;
        }
    }

    static int getChoice(int min_choice, int max_choice) {
        int choice;
        while (true) {
            std::cout << "Enter your choice (" << min_choice << "-" << max_choice << "): ";
            if (!(std::cin >> choice) || choice < min_choice || choice > max_choice) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "❌ Invalid choice. Please enter a number between " << min_choice << " and " << max_choice << ".\n";
                continue;
            }
            return choice;
        }
    }

    static char getYesNo(const std::string& prompt) {
        char choice;
        while (true) {
            std::cout << prompt << " (y/n): ";
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N') {
                return choice;
            }
            std::cout << "❌ Please enter 'y' or 'n'.\n";
        }
    }
};

class FileManager {
public:
    static bool directoryExists(const std::string& path) {
        return !std::ifstream(path).fail();
    }

    static void createDirectoryIfNeeded(const std::string& dir_name) {
        #ifdef _WIN32
            std::string command = "mkdir " + dir_name + " 2>nul";
        #else
            std::string command = "mkdir -p " + dir_name + " 2>/dev/null";
        #endif
        system(command.c_str());
    }

    static void writeToCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "❌ Error: Could not open file " << filename << std::endl;
            return;
        }
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) file << ",";
            }
            file << "\n";
        }
        file.close();
        std::cout << "✅ Results saved to: " << filename << std::endl;
    }

    static void clearDirectory(const std::string& dir_name) {
        #ifdef _WIN32
            std::string command = "del /Q " + dir_name + "\\* 2>nul";
        #else
            std::string command = "rm -rf " + dir_name + "/* 2>/dev/null";
        #endif
        system(command.c_str());
    }
};

class ConsoleUI {
public:
    static void printHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }

    static void printSection(const std::string& section) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "=> " << section << std::endl;
        std::cout << std::string(50, '-') << std::endl;
    }

    static void printSuccess(const std::string& msg) {
        std::cout << "[OK] " << msg << std::endl;
    }

    static void printError(const std::string& msg) {
        std::cout << "[ERROR] " << msg << std::endl;
    }

    static void printInfo(const std::string& msg) {
        std::cout << "[INFO] " << msg << std::endl;
    }

    static void printWarning(const std::string& msg) {
        std::cout << "[WARN] " << msg << std::endl;
    }
};

#endif
