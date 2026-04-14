#include <iostream>
#include <iomanip>// for hex output
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <openssl/des.h>// for DES functions

using namespace std;

//Generate a random 16-bit salt
uint16_t generateSalt() {
    return rand() % 65536;
}

//Convert password into DES key

void passwordToKey(const string& password, DES_cblock &key) {
    // Fill key with zeros first
    memset(key, 0, sizeof(DES_cblock));

    // Copy up to first 8 characters of password
    for (int i = 0; i < 8 && i < password.size(); i++) {
        key[i] = password[i];
    }
}

// Convert DES block to readable hex string
string blockToHex(DES_cblock block) {
    stringstream ss;
    for (int i = 0; i < 8; i++) {
        ss << hex << setw(2) << setfill('0') << (int)(unsigned char)block[i];
    }
    return ss.str();
}

// Encrypt password using: DES ,25 iterations and  salt

string encryptPassword(const string& password, uint16_t salt) {

    DES_cblock key;
    DES_key_schedule schedule;

    // Convert password to DES key
    passwordToKey(password, key);

    // Apply salt (simple way)
    key[0] ^= (salt & 0xFF);        // lower 8 bits
    key[1] ^= ((salt >> 8) & 0xFF); // upper 8 bits

    // Prepare key schedule
    DES_set_key_unchecked(&key, &schedule);

    // Start with 64-bit block of zeros
    DES_cblock block = {0};

    // Apply DES encryption 25 times
    for (int i = 0; i < 25; i++) {
        DES_ecb_encrypt(&block, &block, &schedule, DES_ENCRYPT);
    }

    // Return salt + encrypted block
    return to_string(salt) + ":" + blockToHex(block);
}

//Check if input password matches stored one
bool verifyPassword(const string& inputPassword, const string& stored) {

    // Split salt and hash
    size_t pos = stored.find(":");
    uint16_t salt = stoi(stored.substr(0, pos));

    // Re-encrypt input password using same salt
    string newHash = encryptPassword(inputPassword, salt);

    return newHash == stored;
}


int main() {

    srand(time(0));

    // Example passwords
    vector<string> passwords = {
        "apple123", "hello", "securePass",
        "admin", "qwerty", "pass1234",
        "computer", "network", "crypto", "finaltest"
    };

    vector<string> encryptedList;

    cout << "Encrypted Passwords\n";

    // Generate 10 encrypted passwords
    for (int i = 0; i < 10; i++) {
        uint16_t salt = generateSalt();
        string encrypted = encryptPassword(passwords[i], salt);

        encryptedList.push_back(encrypted);
        cout << i + 1 << ". " << encrypted << endl;
    }

    // Test verification
    cout << "\nPassword Verification Test\n";

    string testPassword = passwords[0];
    string stored = encryptedList[0];

    if (verifyPassword(testPassword, stored)) {
        cout << "Password is CORRECT\n";
    } else {
        cout << "Password is WRONG\n";
    }

    return 0;
}