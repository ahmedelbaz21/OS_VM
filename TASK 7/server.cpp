#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <arpa/inet.h> // for sockaddr_in, inet_pton
#include <unistd.h>    // for close()

using namespace std;

vector<int> clients; // stores all connected client sockets
mutex clientsMutex;  // protects the clients vector for multithreading

// Function to send a message to all clients except the sender
void broadcastMessage(const string &msg, int sender) {
    lock_guard<mutex> lock(clientsMutex); // lock vector to prevent concurrent modification
    for (int client : clients) {
        if (client != sender) {           // don't send message back to the sender
            send(client, msg.c_str(), msg.size(), 0);
        }
    }
}

// Function to handle communication with a single client
void handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer)); // clear buffer
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0); // receive message
        if (bytes <= 0) {                  // if client disconnected
            cout << "Client disconnected\n";
            close(clientSocket);

            // Remove client from the vector
            lock_guard<mutex> lock(clientsMutex);
            clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        }
        string msg(buffer);                 // convert char buffer to string
        cout << "Received: " << msg << endl;
        broadcastMessage(msg, clientSocket); // send to other clients
    }
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create TCP socket
    if (serverSocket == -1) { cerr << "Socket error\n"; return 1; }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);        // server port
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // accept connections from any IP

    // Bind socket to IP and port
    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed\n"; 
        return 1;
    }

    listen(serverSocket, 5); // start listening, max 5 clients in queue
    cout << "Server started on port 8080...\n";

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr); // accept a new client
        if (clientSocket < 0) continue;

        {
            lock_guard<mutex> lock(clientsMutex);
            clients.push_back(clientSocket); // add new client to vector
        }

        cout << "New client connected\n";

        // Start a new thread to handle this client independently
        thread t(handleClient, clientSocket);
        t.detach(); // detach thread so it runs independently
    }

    close(serverSocket); // close server socket (never reached in this infinite loop)
    return 0;
}