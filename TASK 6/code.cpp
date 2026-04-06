#include <iostream>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[]) {

    // Check arguments
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <directory_path> <bin_width>\n";
        return 1;
    }

    string directory = argv[1];
    int binWidth = stoi(argv[2]);

    // Map to store histogram (bin_index -> count)
    map<int, int> histogram;

    try {
        // Traverse directory recursively
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {

            if (fs::is_regular_file(entry.path())) {
                // Get file size
                auto fileSize = ::file_size(entry.path());

                // Determine bin
                int binIndex = fileSize / binWidth;

                // Increment count
                histogram[binIndex]++;
            }
        }

        // Print histogram
        cout << "\nFile Size Histogram:\n";
        for (const auto& [bin, count] : histogram) {
            long lower = bin * binWidth;
            long upper = lower + binWidth - 1;

            cout << "[" << lower << " - " << upper << "] : "
                 << count << " files\n";
        }

    } catch (const fs::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}