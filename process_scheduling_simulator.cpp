#include <iostream>
#include <algorithm>
#include <iomanip>
#include <random>
#include <ctime>
#include <cmath>
#include <queue>
using namespace std;

struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority; // Priority for Priority Scheduling
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int remaining_time; // Remaining time for RR
};

bool compareArrival(process p1, process p2) {
    return p1.arrival_time < p2.arrival_time;
}

bool compareID(process p1, process p2) {
    return p1.pid < p2.pid;
}

bool compareBurstTime(process p1, process p2) {
    return p1.burst_time < p2.burst_time;
}

bool comparePriority(process p1, process p2) {
    return p1.priority < p2.priority;
}


void simulateScheduling(process p[], int n, int context_switch_time,string sortMethod) {

    if(sortMethod=="compareArrival")
        sort(p, p + n, compareArrival);
    else if(sortMethod=="compareID")
        sort(p, p + n, compareID);
    else if(sortMethod=="compareBurstTime")
        sort(p, p + n, compareBurstTime);
    else if(sortMethod=="comparePriority")
        sort(p, p + n, comparePriority);
    else
        return;
    
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int total_idle_time = 0;
    float throughput;

    for (int i = 0; i < n; i++) {
        p[i].start_time = (i == 0) ? p[i].arrival_time : max(p[i - 1].completion_time + context_switch_time, p[i].arrival_time);
        p[i].completion_time = p[i].start_time + p[i].burst_time;
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
        p[i].response_time = p[i].start_time - p[i].arrival_time;

        total_turnaround_time += p[i].turnaround_time;
        total_waiting_time += p[i].waiting_time;
        total_response_time += p[i].response_time;
        total_idle_time += (i == 0) ? p[i].arrival_time : (p[i].start_time - p[i - 1].completion_time - context_switch_time);
    }

    float avg_turnaround_time = (float)total_turnaround_time / n;
    float avg_waiting_time = (float)total_waiting_time / n;
    float avg_response_time = (float)total_response_time / n;
    float cpu_utilization = ((p[n - 1].completion_time - total_idle_time) / (float)p[n - 1].completion_time) * 100;
    throughput = static_cast<float>(n) / (p[n - 1].completion_time - p[0].arrival_time);

    cout << "#P\t" << "AT\t" << "BT\t" << "ST\t" << "CT\t" << "TAT\t" << "WT\t" << "RT\t" << "\n" << endl;

    for (int i = 0; i < n; i++) {
        cout << p[i].pid << "\t" << p[i].arrival_time << "\t" << p[i].burst_time << "\t" << p[i].start_time << "\t"
             << p[i].completion_time << "\t" << p[i].turnaround_time << "\t" << p[i].waiting_time << "\t"
             << p[i].response_time << "\t" << "\n" << endl;
    }
    cout << "Average Turnaround Time = " << avg_turnaround_time << endl;
    cout << "Average Waiting Time = " << avg_waiting_time << endl;
    cout << "Average Response Time = " << avg_response_time << endl;
    cout << "CPU Utilization = " << cpu_utilization << "%" << endl;
    cout<<fixed<<setprecision(5);
    cout << "Throughput = " << throughput << " process/unit time" << endl;
}

void simulateRRScheduling(process p[], int n, int time_quantum, int context_switch_time) {
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int total_idle_time = 0;
    float throughput;

    queue<process> rr_queue;

    int current_time = p[0].arrival_time;
    int completed_processes = 0;

    int i = 0;
    while (completed_processes < n) {
        while (i < n && p[i].arrival_time <= current_time) {
            rr_queue.push(p[i]);
            i++;
        }

        if (!rr_queue.empty()) {
            process current_process = rr_queue.front();
            rr_queue.pop();

            if (current_process.remaining_time <= time_quantum) {
                current_time += current_process.remaining_time;
                current_process.remaining_time = 0;
                current_process.completion_time = current_time;
                completed_processes++;

                p[current_process.pid - 1] = current_process;
            } else {
                current_time += time_quantum;
                current_process.remaining_time -= time_quantum;
                rr_queue.push(current_process);
            }
        } else {
            current_time = p[i].arrival_time;
        }

        current_time += context_switch_time; // Context switch time
    }

    for (int i = 0; i < n; i++) {
        p[i].start_time = max(p[i].start_time, p[i].arrival_time);
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
        p[i].response_time = p[i].start_time - p[i].arrival_time;

        total_turnaround_time += p[i].turnaround_time;
        total_waiting_time += p[i].waiting_time;
        total_response_time += p[i].response_time;
        total_idle_time += (i == 0) ? p[i].arrival_time : (p[i].start_time - p[i - 1].completion_time - context_switch_time);
    }

    float avg_turnaround_time = (float)total_turnaround_time / n;
    float avg_waiting_time = (float)total_waiting_time / n;
    float avg_response_time = (float)total_response_time / n;
    float cpu_utilization = ((p[n - 1].completion_time - total_idle_time) / (float)p[n - 1].completion_time) * 100;
    throughput = float(n)/(p[n - 1].completion_time - p[0].arrival_time);

    cout<<float(n)<<" "<<p[n-1].completion_time<<" "<<p[0].arrival_time<<endl;
    cout << "#P\t" << "AT\t" << "BT\t" << "ST\t" << "CT\t" << "TAT\t" << "WT\t" << "RT\t" << "\n" << endl;

    for (int i = 0; i < n; i++) {
        cout << p[i].pid << "\t" << p[i].arrival_time << "\t" << p[i].burst_time << "\t" << p[i].start_time << "\t"
             << p[i].completion_time << "\t" << p[i].turnaround_time << "\t" << p[i].waiting_time << "\t"
             << p[i].response_time << "\t" << "\n" << endl;
    }
    cout << "Average Turnaround Time = " << avg_turnaround_time << endl;
    cout << "Average Waiting Time = " << avg_waiting_time << endl;
    cout << "Average Response Time = " << avg_response_time << endl;
    cout << "CPU Utilization = " << cpu_utilization << "%" << endl;
    cout<<fixed<<setprecision(5);
    cout << "Throughput = " << throughput << " process/unit time" << endl;
}


int main() {
    int n;
    struct process p[100];
    float avg_turnaround_time;
    float avg_waiting_time;
    float avg_response_time;
    float cpu_utilization;
    float throughput;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int total_idle_time = 0;

    // Set random seed based on current time
    srand(static_cast<unsigned>(time(nullptr)));

    cout << setprecision(2) << fixed;

    cout << "Enter the number of processes: ";
    cin >> n;

    // Generate random arrival times, CPU burst times, and priorities
    for (int i = 0; i < n; i++) {
        p[i].arrival_time = rand() % 3501 + 500; // Range: 500-4000 ms
        p[i].burst_time = rand() % 3501 + 500;   // Range: 500-4000 ms
        p[i].priority = rand() % 10;            // Range: 0-9 for Priority Scheduling
        p[i].pid = i + 1;
        p[i].remaining_time = p[i].burst_time; // Initialize remaining time for SRT and RR
    }

    

    int context_switch_time = 100; // 100ms context switch time

    // Simulate FCFS
    cout << "First Come First Serve (FCFS):" << endl;
    simulateScheduling(p, n, context_switch_time, "compareArrival");

    // Simulate SJN (Shortest Job Next)
    cout << "\nShortest Job Next (SJN):" << endl;
    simulateScheduling(p, n, context_switch_time, "compareBurstTime");

    // Simulate Priority Scheduling
    cout << "\nPriority Scheduling:" << endl;
    simulateScheduling(p, n, context_switch_time, "comparePriority");

    // Simulate Round Robin (RR)
    cout << "\nRound Robin (RR):" << endl;
    int time_quantum = 500; // Time quantum for RR (500 ms)
    simulateRRScheduling(p, n, time_quantum, context_switch_time);

    return 0;
}
