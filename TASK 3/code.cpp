#include <iostream>
#include <fstream>      // used for file handling
#include <vector>
#include <unordered_map> // used to store pages in memory
#include <limits>       // used for finding minimum age value

using namespace std;

// This function simulates the Aging page replacement algorithm  and returns page faults per 1000 references
double simulateAging(vector<int>& references, int numFrames) {

    unordered_map<int, unsigned int> memory; // page -> age counter
    vector<int> frames;  // stores current pages in memory
    int pageFaults = 0;

    // loop over all page references
    for (int page : references) {

        // shift age counters for all pages in memory
        for (int p : frames) {
            memory[p] >>= 1;   // shift right by 1
        }

        // check if page is already in memory
        if (memory.find(page) != memory.end()) {

            // page hit, set MSB to indicate recent use
            memory[page] |= (1 << 7);
        }
        else {
            // page fault
            pageFaults++;

            // if free frame available
            if (frames.size() < numFrames) {

                frames.push_back(page);
                memory[page] = (1 << 7); // initialize age with MSB set
            }
            else {
                // find page with smallest age (victim)
                int victim = -1;
                unsigned int minAge = numeric_limits<unsigned int>::max();

                for (int p : frames) {
                    if (memory[p] < minAge) {
                        minAge = memory[p];
                        victim = p;
                    }
                }

                // remove victim
                memory.erase(victim);

                for (int i = 0; i < frames.size(); i++) {
                    if (frames[i] == victim) {
                        frames[i] = page;
                        break;
                    }
                }

                // insert new page
                memory[page] = (1 << 7);
            }
        }
    }

    // calculate faults per 1000 references
    double faultsPer1000 =
        ((double)pageFaults / references.size()) * 1000.0;

    return faultsPer1000;
}

int main() {
    string fileName;
    int maxFrames;

    cout << "Enter file name: ";
    cin >> fileName;
    cout << "Enter maximum number of frames: ";
    cin >> maxFrames;

    ifstream file(fileName);

    if (!file) {
        cout << "Error opening file\n";
        return 1;
    }

    // read all page references from file
    vector<int> references;
    int page;
    
    // loop to read integers until end of file
    while (file >> page) {
        references.push_back(page);
    }

    file.close();

    if (references.size() == 0) {
        cout << "File is empty\n";
        return 0;
    }

    cout << "\nFrames , Faults per 1000 references\n";

    // test from 1 frame up to maxFrames
    for (int frames = 1; frames <= maxFrames; frames++) {

        double result = simulateAging(references, frames);

        cout << frames << " , " << result << endl;
    }

    return 0;
}