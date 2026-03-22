#include <iostream>
#include <fstream>        // used for file handling
#include <vector>
#include <queue>          // used for Round Robin
#include <algorithm>      // used for sorting

using namespace std;

// process structure
struct Process {
    int pid;
    int arrival;
    int burst;
};

// FCFS 
// This function simulates First Come First Serve scheduling
double simulateFCFS(vector<Process>& processes) {

    // sort processes by arrival time
    sort(processes.begin(), processes.end(),
         [](Process a, Process b) {
             return a.arrival < b.arrival;
         });

    int time = 0;
    double totalWaiting = 0;

    // loop over all processes
    for (auto p : processes) {

        if (time < p.arrival) {
            time = p.arrival;
        }

        int waiting = time - p.arrival;
        totalWaiting += waiting;

        time += p.burst;
    }

    return totalWaiting / processes.size();
}

// SJF
// This function simulates Shortest Job First scheduling
double simulateSJF(vector<Process> processes) {

    int n = processes.size();
    int time = 0;
    double totalWaiting = 0;
    vector<bool> completed(n, false);

    int finished = 0;

    // loop until all processes finish
    while (finished < n) {

        int idx = -1;
        int minBurst = 1e9;

        // find shortest job available at current time
        for (int i = 0; i < n; i++) {
            if (!completed[i] &&
                processes[i].arrival <= time &&
                processes[i].burst < minBurst) {

                minBurst = processes[i].burst;
                idx = i;
            }
        }

        // if no process is ready
        if (idx == -1) {
            time++;
            continue;
        }

        int waiting = time - processes[idx].arrival;
        totalWaiting += waiting;

        time += processes[idx].burst;
        completed[idx] = true;
        finished++;
    }

    return totalWaiting / n;
}

// Round Robin
// This function simulates Round Robin scheduling
double simulateRoundRobin(vector<Process> processes, int quantum) {

    queue<int> q;  // stores indices of processes
    int n = processes.size();

    vector<int> remaining(n);
    vector<int> lastExec(n);
    vector<int> waiting(n, 0);

    // initialize remaining burst time
    for (int i = 0; i < n; i++) {
        remaining[i] = processes[i].burst;
        lastExec[i] = processes[i].arrival;
    }

    int time = 0;
    int i = 0;

    // sort by arrival time
    sort(processes.begin(), processes.end(),
         [](Process a, Process b) {
             return a.arrival < b.arrival;
         });

    // loop while processes remain
    while (i < n || !q.empty()) {

        // add arrived processes to queue
        while (i < n && processes[i].arrival <= time) {
            q.push(i);
            i++;
        }

        // if queue empty, jump time
        if (q.empty()) {
            time = processes[i].arrival;
            continue;
        }

        int idx = q.front();
        q.pop();

        waiting[idx] += time - lastExec[idx];

        int execTime = min(quantum, remaining[idx]);
        time += execTime;
        remaining[idx] -= execTime;

        lastExec[idx] = time;

        // add new arrivals during execution
        while (i < n && processes[i].arrival <= time) {
            q.push(i);
            i++;
        }

        // if process not finished, push again
        if (remaining[idx] > 0) {
            q.push(idx);
        }
    }

    double totalWaiting = 0;
    for (int w : waiting) {
        totalWaiting += w;
    }

    return totalWaiting / n;
}


int main() {

    string fileName;
    int quantum;

    cout << "Enter file name: ";
    cin >> fileName;

    cout << "Enter time quantum (for Round Robin): ";
    cin >> quantum;

    ifstream file(fileName);

    if (!file) {
        cout << "Error opening file\n";
        return 1;
    }

    vector<Process> processes;
    int at, bt;
    int pid = 1;

    // read processes from file (arrival time, burst time)
    while (file >> at >> bt) {
        processes.push_back({pid++, at, bt});
    }

    file.close();

    if (processes.size() == 0) {
        cout << "File is empty\n";
        return 0;
    }

    // run simulations
    double fcfs = simulateFCFS(processes);
    double sjf = simulateSJF(processes);
    double rr = simulateRoundRobin(processes, quantum);

    cout << "\nAlgorithm , Average Waiting Time\n";
    cout << "FCFS , " << fcfs << endl;
    cout << "SJF , " << sjf << endl;
    cout << "Round Robin , " << rr << endl;

    return 0;
}