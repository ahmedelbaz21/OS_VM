#include <iostream>
#include <fstream>      // used for file handling
#include <vector>

using namespace std;

int main() {

    ifstream file("input.txt"); // open input file containing system data

    int n, m; // n = number of processes, m = number of resource types

    file >> n >> m; // read number of processes and resource types

    vector<int> E(m); // vector for existing resources of each type
    vector<int> A(m); // vector for available resources

    // matrices for current allocation and remaining requests
    vector<vector<int>> C(n, vector<int>(m)); 
    vector<vector<int>> R(n, vector<int>(m)); 

    // read existing resources vector E
    for (int j = 0; j < m; j++)
        file >> E[j];

    // read allocation matrix C
    // each row represents resources currently allocated to process i
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            file >> C[i][j];
            
    // read request matrix R
    // each row represents resources still requested by process i
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            file >> R[i][j];

    // calculate available resources
    // A = E - sum of allocated resources
    for (int j = 0; j < m; j++) {

        int sum = 0;

        for (int i = 0; i < n; i++)
            sum += C[i][j]; // sum allocations for resource type j

        A[j] = E[j] - sum; // available resources of type j
    }

    // Finish vector tracks if process can complete
    vector<bool> Finish(n, false);

    bool found = true; // used to repeat the search for runnable processes

    // deadlock detection loop
    while (found) {

        found = false;

        // check each process
        for (int i = 0; i < n; i++) {

            // process not finished yet
            if (!Finish[i]) {

                bool canRun = true;

                // check if process requests can be satisfied with available resources
                for (int j = 0; j < m; j++) {

                    if (R[i][j] > A[j]) {
                        canRun = false; // not enough resources
                        break;
                    }
                }

                // if process can run
                if (canRun) {

                    // simulate process finishing and releasing resources
                    for (int j = 0; j < m; j++)
                        A[j] += C[i][j];

                    Finish[i] = true; // mark process as finished
                    found = true;     // continue searching
                }
            }
        }
    }

    bool deadlock = false;

    // check if any process could not finish
    for (int i = 0; i < n; i++) {

        if (!Finish[i]) {
            deadlock = true;
        }
    }

    // output results
    if (deadlock) {

        cout << "Deadlock detected.\nProcesses in deadlock: ";

        for (int i = 0; i < n; i++) {

            if (!Finish[i])
                cout << "P" << i << " ";
        }

        cout << endl;
    }
    else {

        cout << "No deadlock detected." << endl;
    }

    return 0;
}