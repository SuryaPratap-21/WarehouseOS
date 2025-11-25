#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H

#include "utilities.h"
#include <algorithm>
#include <cmath>

class DiskScheduler {
private:
    std::vector<DiskRequest> requests;
    int head_position;
    int disk_size;
    std::vector<int> head_sequence;
    int total_seek_time;

public:
    DiskScheduler(const std::vector<DiskRequest>& reqs, int head_pos, int size)
        : requests(reqs), head_position(head_pos), disk_size(size), total_seek_time(0) {}

    void runFCFS() {
        ConsoleUI::printSection("FCFS Disk Scheduling - Trucks Dispatched by Arrival Order");
        head_sequence.clear();
        total_seek_time = 0;
        head_sequence.push_back(head_position);

        std::sort(requests.begin(), requests.end(),
                 [](const DiskRequest& a, const DiskRequest& b) { return a.arrival_time < b.arrival_time; });

        for (const auto& req : requests) {
            int seek = std::abs(req.cylinder - head_position);
            total_seek_time += seek;
            head_position = req.cylinder;
            head_sequence.push_back(head_position);
        }

        displayResults("FCFS");
        saveToCSV("output/fcfs_disk_results.csv");
    }

    void runSSTF() {
        ConsoleUI::printSection("SSTF Disk Scheduling - Nearest Dock Served First");
        head_sequence.clear();
        total_seek_time = 0;
        head_sequence.push_back(head_position);

        std::vector<bool> visited(requests.size(), false);

        for (size_t i = 0; i < requests.size(); ++i) {
            int nearest = -1;
            int min_distance = INT_MAX;

            for (size_t j = 0; j < requests.size(); ++j) {
                if (!visited[j]) {
                    int distance = std::abs(requests[j].cylinder - head_position);
                    if (distance < min_distance) {
                        min_distance = distance;
                        nearest = j;
                    }
                }
            }

            if (nearest != -1) {
                visited[nearest] = true;
                total_seek_time += min_distance;
                head_position = requests[nearest].cylinder;
                head_sequence.push_back(head_position);
            }
        }

        displayResults("SSTF");
        saveToCSV("output/sstf_disk_results.csv");
    }

    void runSCAN() {
        ConsoleUI::printSection("SCAN Disk Scheduling - Forklift Moves in One Direction");
        char direction = InputValidator::getYesNo("Start moving towards higher dock numbers");
        bool move_up = (direction == 'y' || direction == 'Y');

        head_sequence.clear();
        total_seek_time = 0;
        head_sequence.push_back(head_position);

        std::vector<int> cylinders;
        for (const auto& req : requests) {
            cylinders.push_back(req.cylinder);
        }
        std::sort(cylinders.begin(), cylinders.end());

        if (move_up) {
            for (int cyl : cylinders) {
                if (cyl >= head_position) {
                    total_seek_time += std::abs(cyl - head_position);
                    head_position = cyl;
                    head_sequence.push_back(head_position);
                }
            }
            for (int i = (int)cylinders.size() - 1; i >= 0; --i) {
                if (cylinders[i] < head_position) {
                    total_seek_time += std::abs(cylinders[i] - head_position);
                    head_position = cylinders[i];
                    head_sequence.push_back(head_position);
                }
            }
        } else {
            for (int i = (int)cylinders.size() - 1; i >= 0; --i) {
                if (cylinders[i] <= head_position) {
                    total_seek_time += std::abs(cylinders[i] - head_position);
                    head_position = cylinders[i];
                    head_sequence.push_back(head_position);
                }
            }
            for (int cyl : cylinders) {
                if (cyl > head_position) {
                    total_seek_time += std::abs(cyl - head_position);
                    head_position = cyl;
                    head_sequence.push_back(head_position);
                }
            }
        }

        displayResults("SCAN");
        saveToCSV("output/scan_disk_results.csv");
    }

    void runCSCAN() {
        ConsoleUI::printSection("C-SCAN Disk Scheduling - Circular Forklift Movement");
        char direction = InputValidator::getYesNo("Start moving towards higher dock numbers");
        bool move_up = (direction == 'y' || direction == 'Y');

        head_sequence.clear();
        total_seek_time = 0;
        head_sequence.push_back(head_position);

        std::vector<int> cylinders;
        for (const auto& req : requests) {
            cylinders.push_back(req.cylinder);
        }
        std::sort(cylinders.begin(), cylinders.end());

        if (move_up) {
            for (int cyl : cylinders) {
                if (cyl >= head_position) {
                    total_seek_time += std::abs(cyl - head_position);
                    head_position = cyl;
                    head_sequence.push_back(head_position);
                }
            }
            total_seek_time += std::abs(disk_size - head_position);
            total_seek_time += disk_size;
            head_position = 0;
            head_sequence.push_back(head_position);
            for (int cyl : cylinders) {
                if (cyl < head_position) {
                    total_seek_time += std::abs(cyl - head_position);
                    head_position = cyl;
                    head_sequence.push_back(head_position);
                }
            }
        } else {
            for (int i = (int)cylinders.size() - 1; i >= 0; --i) {
                if (cylinders[i] <= head_position) {
                    total_seek_time += std::abs(cylinders[i] - head_position);
                    head_position = cylinders[i];
                    head_sequence.push_back(head_position);
                }
            }
            total_seek_time += head_position;
            head_position = disk_size;
            head_sequence.push_back(head_position);
            total_seek_time += disk_size;
            for (int i = (int)cylinders.size() - 1; i >= 0; --i) {
                if (cylinders[i] > head_position) {
                    total_seek_time += std::abs(cylinders[i] - head_position);
                    head_position = cylinders[i];
                    head_sequence.push_back(head_position);
                }
            }
        }

        displayResults("C-SCAN");
        saveToCSV("output/cscan_disk_results.csv");
    }

private:
    void displayResults(const std::string& algorithm) {
        ConsoleUI::printSection("DISK SCHEDULING RESULTS - " + algorithm);
        std::cout << "Head Sequence: ";
        for (size_t i = 0; i < head_sequence.size(); ++i) {
            std::cout << head_sequence[i];
            if (i < head_sequence.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";

        std::cout << "Total Seek Time: " << total_seek_time << " units\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Avg Seek Time: " << (requests.empty() ? 0 : total_seek_time / (double)requests.size()) << " units\n";
    }

    void saveToCSV(const std::string& filename) {
        std::vector<std::vector<std::string>> csv_data;
        csv_data.push_back({"Truck_Request_ID", "Dock_Cylinder", "Arrival_Time"});

        for (const auto& req : requests) {
            csv_data.push_back({
                std::to_string(req.request_id),
                std::to_string(req.cylinder),
                std::to_string(req.arrival_time)
            });
        }

        csv_data.push_back({""});
        csv_data.push_back({"Head Sequence:"});
        std::string seq = "";
        for (int cyl : head_sequence) {
            seq += std::to_string(cyl) + " -> ";
        }
        if (seq.length() > 4) seq = seq.substr(0, seq.length() - 4);
        csv_data.push_back({seq});
        csv_data.push_back({"Total Seek Time", std::to_string(total_seek_time)});
        csv_data.push_back({"Avg Seek Time", std::to_string(requests.empty() ? 0 : total_seek_time / (double)requests.size())});

        FileManager::writeToCSV(filename, csv_data);
    }
};

#endif
