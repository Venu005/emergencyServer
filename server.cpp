#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <map>

#define PORT 8080
#define BUFFER_SIZE 1024

// Emergency service numbers
std::map<int, std::string> emergencyNumbers = {
    {1, "Police Station: 100"},
    {2, "Ambulance: 102"},
    {3, "Fire Station: 101"},
    {4, "Vehicle Repair: 1800-123-456"},
    {5, "Food Delivery: 1800-FOOD-123"},
    {6, "Blood Bank: 1800-BLOOD-999"}
};

// Function to handle each client query in a separate thread
void* handleClient(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (n < 0) {
            std::cerr << "Failed to receive data from client." << std::endl;
            continue;
        }

        int query = atoi(buffer);  // Convert query to integer
        std::string response = "Invalid query";

        // Find the emergency number corresponding to the query
        if (emergencyNumbers.find(query) != emergencyNumbers.end()) {
            response = emergencyNumbers[query];
        }

        // Send response back to client
        sendto(sock, response.c_str(), response.size(), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
    }

    close(sock);
    pthread_exit(nullptr);
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // Set up server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Listen on any interface
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(sockfd, (const struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed." << std::endl;
        close(sockfd);
        return -1;
    }

    std::cout << "Emergency server is running and listening on port " << PORT << std::endl;

    // Create a thread to handle client queries
    pthread_t thread_id;
    if (pthread_create(&thread_id, nullptr, handleClient, (void*)&sockfd) != 0) {
        std::cerr << "Failed to create thread." << std::endl;
        return -1;
    }

    // Wait for the thread to finish (it won't in this case, as it handles clients in a loop)
    pthread_join(thread_id, nullptr);

    close(sockfd);
    return 0;
}

