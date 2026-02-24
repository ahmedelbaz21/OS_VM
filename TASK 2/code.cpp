#include <iostream>
#include <fstream> // used for file handling
#include <vector>
#include <string>
#include <unordered_map> // used for counting words
#include <thread> // used for multithreading
#include <cctype> // used for isalnum and tolower (charaxcter handling)

using namespace std;

// This function is executed by each thread. It counts words in a specific range of lines.
void countWords(vector<string>& lines, int start, int end, unordered_map<string, int>& result) {

    unordered_map<string, int> localMap;  // each thread has its own map

    // loops over lines from start to end
    for (int i = start; i < end; i++) {
        string word = "";
        for (char c : lines[i]) {
            if (isalnum(c)) {
                word += tolower(c);   // convert to lowercase
            }
            else {
                if (word != "") {
                    localMap[word]++;
                    word = "";
                }
            }
        }
        // last word in line
        if (word != "") {
            localMap[word]++;
        }
    }

    // store the result in the shared vector
    result = localMap;
}

int main() {

    string fileName;
    int N;

    cout << "Enter file name: ";
    cin >> fileName;

    cout << "Enter number of threads: ";
    cin >> N;

    ifstream file(fileName);

    if (!file) {
        cout << "Error opening file\n";
        return 1;
    }

    // Read file line by line
    vector<string> lines;
    string line;

    while (getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    if (lines.size() == 0) {
        cout << "File is empty\n";
        return 0;
    }

    // If threads > lines, reduce threads
    if (N > lines.size()) {
        N = lines.size();
    }

    // Create a vector to hold partial results from each thread
    vector<unordered_map<string, int>> partial(N);
    vector<thread> threads;

    int totalLines = lines.size();
    int chunk = totalLines / N;
    int remainder = totalLines % N;

    int start = 0;

    // Create threads (each thread gets a segment of lines)
    for (int i = 0; i < N; i++) {

        int end = start + chunk;

        if (i < remainder) {
            end++;   // distribute extra lines
        }

        threads.push_back(thread(countWords, ref(lines), start, end, ref(partial[i])));

        start = end;
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; i++) {
        threads[i].join();
    }

    // Merge results
    unordered_map<string, int> finalResult;

    for (int i = 0; i < N; i++) {
        for (auto& pair : partial[i]) {
            finalResult[pair.first] += pair.second;
        }
    }

    // Print final word counts
    cout << "\nFinal Word Count:\n";

    for (auto& pair : finalResult) {
        cout << pair.first << " : " << pair.second << endl;
    }

    return 0;
}