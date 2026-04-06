#include <iostream>
#include <thread>
#include <cstring>
#include <arpa/inet.h> // for sockaddr_in, inet_pton
#include <unistd.h>    // for close()

using namespace std;

// Thread function to constantly receive messages from server
void receiveMessages(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));      // clear buffer
        int bytes = recv(sock, buffer, sizeof(buffer), 0); // receive message
        if (bytes <= 0) {                       // if server disconnected
            cout << "\n[Server disconnected]\n";
            exit(0);                             // exit program
        }
        cout << "\nMessage: " << buffer << endl; // print received message
        cout << "> "; cout.flush();             // reprint prompt for typing
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create TCP socket
    if (sock == -1) { cerr << "Socket error\n"; return 1; }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);       // server port
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // server IP (localhost)

    // Connect to server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connect failed\n"; return 1;
    }

    cout << "Connected to server. Start typing...\n";

    // Start receive thread
    thread t(receiveMessages, sock);
    t.detach(); // detach so it runs independently

    string msg;
    while (true) {
        cout << "> "; getline(cin, msg);     // read user input
        send(sock, msg.c_str(), msg.size(), 0); // send to server
    }

    close(sock); // close client socket (never reached)
    return 0;
}