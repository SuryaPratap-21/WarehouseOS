#ifndef CPU_SCHEDULER_H
#define CPU_SCHEDULER_H

#include "utilities.h"
#include <queue>
#include <numeric>
#include <set>

class CPUScheduler {
private:
    std::vector<Process> processes;
    std::vector<Process> completed;
    double cpu_utilization;
    int total_time;

public:
    CPUScheduler(const std::vector<Process>& procs) : processes(procs), cpu_utilization(0), total_time(0) {}

    void runFCFS() {
        ConsoleUI::printSection("FCFS Scheduling - Orders Handled by Arrival Sequence");
        completed.clear();
        completed = processes;
        std::sort(completed.begin(), completed.end(),
                 [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });

        int current_time = 0;
        for (auto& p : completed) {
            if (current_time < p.arrival_time) current_time = p.arrival_time;
            p.completion_time = current_time + p.burst_time;
            p.waiting_time = current_time - p.arrival_time;
            p.turnaround_time = p.completion_time - p.arrival_time;
            current_time = p.completion_time;
        }
        total_time = current_time;
        displayResults("FCFS");
        saveToCSV("output/fcfs_cpu_results.csv");
    }

    void runSJF() {
        ConsoleUI::printSection("SJF Scheduling - Smallest Orders Processed First");
        completed.clear();
        completed = processes;

        int current_time = 0;
        for (size_t i = 0; i < completed.size(); ++i) {
            auto remaining = completed.begin() + i;
            for (auto it = remaining; it != completed.end(); ++it) {
                if (it->arrival_time <= current_time &&
                    it->burst_time < remaining->burst_time) {
                    std::swap(*remaining, *it);
                }
            }
            if (current_time < completed[i].arrival_time) {
                current_time = completed[i].arrival_time;
            }
            completed[i].completion_time = current_time + completed[i].burst_time;
            completed[i].waiting_time = current_time - completed[i].arrival_time;
            completed[i].turnaround_time = completed[i].completion_time - completed[i].arrival_time;
            current_time = completed[i].completion_time;
        }
        total_time = current_time;
        displayResults("SJF");
        saveToCSV("output/sjf_cpu_results.csv");
    }

    void runSRJF() {
        ConsoleUI::printSection("SRJF Scheduling - Preemptive Shortest Job First");
        completed = processes;
        std::sort(completed.begin(), completed.end(),
                 [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });

        std::vector<int> remaining_time(completed.size());
        for (size_t i = 0; i < completed.size(); ++i) {
            remaining_time[i] = completed[i].burst_time;
        }

        int current_time = 0;
        int processed = 0;

        while (processed < (int)completed.size()) {
            int next_process = -1;
            int min_remaining = INT_MAX;

            for (size_t i = 0; i < completed.size(); ++i) {
                if (completed[i].arrival_time <= current_time && remaining_time[i] > 0 &&
                    remaining_time[i] < min_remaining) {
                    next_process = i;
                    min_remaining = remaining_time[i];
                }
            }

            if (next_process == -1) {
                int next_arrival = INT_MAX;
                for (size_t i = 0; i < completed.size(); ++i) {
                    if (remaining_time[i] > 0 && completed[i].arrival_time < next_arrival) {
                        next_arrival = completed[i].arrival_time;
                    }
                }
                if (next_arrival != INT_MAX) {
                    current_time = next_arrival;
                }
                continue;
            }

            remaining_time[next_process]--;
            current_time++;

            if (remaining_time[next_process] == 0) {
                completed[next_process].completion_time = current_time;
                completed[next_process].waiting_time = current_time - completed[next_process].burst_time - completed[next_process].arrival_time;
                completed[next_process].turnaround_time = current_time - completed[next_process].arrival_time;
                processed++;
            }
        }

        total_time = current_time;
        displayResults("SRJF");
        saveToCSV("output/srjf_cpu_results.csv");
    }

    void runPriorityScheduling(bool preemptive) {
        std::string mode = preemptive ? "PREEMPTIVE" : "NON-PREEMPTIVE";
        ConsoleUI::printSection("Priority Scheduling - " + mode + " (Higher Number = Higher Priority)");
        completed = processes;

        if (!preemptive) {
            runNonPreemptivePriority();
        } else {
            runPreemptivePriority();
        }

        total_time = completed.back().completion_time;
        displayResults("PRIORITY_" + mode);
        saveToCSV("output/priority_" + (preemptive ? std::string("preemptive") : std::string("non_preemptive")) + "_cpu_results.csv");
    }

private:
    void runNonPreemptivePriority() {
        int current_time = 0;
        std::vector<bool> executed(completed.size(), false);

        for (size_t i = 0; i < completed.size(); ++i) {
            int next_process = -1;
            int highest_priority = -1;

            for (size_t j = 0; j < completed.size(); ++j) {
                if (!executed[j] && completed[j].arrival_time <= current_time &&
                    completed[j].priority > highest_priority) {
                    next_process = j;
                    highest_priority = completed[j].priority;
                }
            }

            if (next_process == -1) {
                for (size_t j = 0; j < completed.size(); ++j) {
                    if (!executed[j]) {
                        current_time = completed[j].arrival_time;
                        next_process = j;
                        break;
                    }
                }
            }

            if (next_process != -1) {
                current_time += completed[next_process].burst_time;
                completed[next_process].completion_time = current_time;
                completed[next_process].waiting_time = current_time - completed[next_process].burst_time - completed[next_process].arrival_time;
                completed[next_process].turnaround_time = completed[next_process].completion_time - completed[next_process].arrival_time;
                executed[next_process] = true;
            }
        }
    }

    void runPreemptivePriority() {
        std::vector<int> remaining_time(completed.size());
        for (size_t i = 0; i < completed.size(); ++i) {
            remaining_time[i] = completed[i].burst_time;
        }

        int current_time = 0;
        int processed = 0;

        while (processed < (int)completed.size()) {
            int next_process = -1;
            int highest_priority = -1;

            for (size_t i = 0; i < completed.size(); ++i) {
                if (completed[i].arrival_time <= current_time && remaining_time[i] > 0 &&
                    completed[i].priority > highest_priority) {
                    next_process = i;
                    highest_priority = completed[i].priority;
                }
            }

            if (next_process == -1) {
                int next_arrival = INT_MAX;
                for (size_t i = 0; i < completed.size(); ++i) {
                    if (remaining_time[i] > 0 && completed[i].arrival_time < next_arrival) {
                        next_arrival = completed[i].arrival_time;
                    }
                }
                if (next_arrival != INT_MAX) {
                    current_time = next_arrival;
                }
                continue;
            }

            remaining_time[next_process]--;
            current_time++;

            if (remaining_time[next_process] == 0) {
                completed[next_process].completion_time = current_time;
                completed[next_process].waiting_time = current_time - completed[next_process].burst_time - completed[next_process].arrival_time;
                completed[next_process].turnaround_time = current_time - completed[next_process].arrival_time;
                processed++;
            }
        }
    }

public:
    void runRoundRobin() {
        int time_quantum = InputValidator::getPositiveInteger("Enter time quantum (time slice per order): ", 1000);
        ConsoleUI::printSection("Round Robin Scheduling - Time Quantum: " + std::to_string(time_quantum));

        completed = processes;
        std::sort(completed.begin(), completed.end(),
                 [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });

        std::vector<int> remaining_time(completed.size());
        for (size_t i = 0; i < completed.size(); ++i) {
            remaining_time[i] = completed[i].burst_time;
        }

        std::queue<int> ready_queue;
        int current_time = 0;
        int processed = 0;
        std::set<int> in_queue;

        for (size_t i = 0; i < completed.size(); ++i) {
            if (completed[i].arrival_time <= current_time) {
                ready_queue.push(i);
                in_queue.insert(i);
            }
        }

        while (processed < (int)completed.size()) {
            if (ready_queue.empty()) {
                for (size_t i = 0; i < completed.size(); ++i) {
                    if (remaining_time[i] > 0 && completed[i].arrival_time > current_time) {
                        current_time = completed[i].arrival_time;
                        ready_queue.push(i);
                        in_queue.insert(i);
                        break;
                    }
                }
                continue;
            }

            int process_idx = ready_queue.front();
            ready_queue.pop();
            in_queue.erase(process_idx);

            int execution_time = std::min(time_quantum, remaining_time[process_idx]);
            current_time += execution_time;
            remaining_time[process_idx] -= execution_time;

            for (size_t i = 0; i < completed.size(); ++i) {
                if (remaining_time[i] > 0 && completed[i].arrival_time <= current_time &&
                    in_queue.find(i) == in_queue.end()) {
                    ready_queue.push(i);
                    in_queue.insert(i);
                }
            }

            if (remaining_time[process_idx] > 0) {
                ready_queue.push(process_idx);
                in_queue.insert(process_idx);
            } else {
                completed[process_idx].completion_time = current_time;
                completed[process_idx].waiting_time = current_time - completed[process_idx].burst_time - completed[process_idx].arrival_time;
                completed[process_idx].turnaround_time = completed[process_idx].completion_time - completed[process_idx].arrival_time;
                processed++;
            }
        }

        total_time = current_time;
        displayResults("Round Robin (TQ=" + std::to_string(time_quantum) + ")");
        saveToCSV("output/rr_cpu_results.csv");
    }

    void displayResults(const std::string& algorithm) {
        double avg_waiting = 0, avg_turnaround = 0;
        int total_burst = 0;

        for (const auto& p : completed) {
            avg_waiting += p.waiting_time;
            avg_turnaround += p.turnaround_time;
            total_burst += p.burst_time;
        }

        avg_waiting /= completed.size();
        avg_turnaround /= completed.size();
        cpu_utilization = (total_burst * 100.0) / total_time;

        ConsoleUI::printSection("SCHEDULING RESULTS - " + algorithm);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total Time: " << total_time << " units\n";
        std::cout << "CPU Utilization: " << cpu_utilization << "%\n";
        std::cout << "Avg Waiting Time: " << avg_waiting << " units\n";
        std::cout << "Avg Turnaround Time: " << avg_turnaround << " units\n";
        std::cout << "Throughput: " << (completed.size() * 1.0 / total_time) << " orders/unit\n";
    }

    void saveToCSV(const std::string& filename) {
        std::vector<std::vector<std::string>> csv_data;
        csv_data.push_back({"Order_ID", "Arrival_Time", "Burst_Time", "Priority", "Completion_Time", "Waiting_Time", "Turnaround_Time"});

        for (const auto& p : completed) {
            csv_data.push_back({
                std::to_string(p.id),
                std::to_string(p.arrival_time),
                std::to_string(p.burst_time),
                std::to_string(p.priority),
                std::to_string(p.completion_time),
                std::to_string(p.waiting_time),
                std::to_string(p.turnaround_time)
            });
        }

        std::string gantt_chart = "Gantt Chart: |";
        for (const auto& p : completed) {
            gantt_chart += "P" + std::to_string(p.id) + "|";
        }
        csv_data.push_back({gantt_chart});

        std::cout << "\n" << gantt_chart << "\n";

        FileManager::writeToCSV(filename, csv_data);
    }
};

#endif
