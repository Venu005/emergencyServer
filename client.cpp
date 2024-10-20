#include <iostream>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <unistd.h>  // For getting the process ID
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to generate unique cache file name based on the process ID (PID)
std::string getCacheFileName() {
    pid_t pid = getpid();  // Get the current process ID
    return "cache_" + std::to_string(pid) + ".txt";
}

// Function to load the cache from a file
std::unordered_map<int, std::string> loadCache(const std::string& cacheFileName) {
    std::unordered_map<int, std::string> cache;
    std::ifstream cacheFile(cacheFileName);
    if (cacheFile.is_open()) {
        int query;
        std::string response;
        while (cacheFile >> query) {
            std::getline(cacheFile, response);
            if (!response.empty()) {
                cache[query] = response;
            }
        }
        cacheFile.close();
    }
    return cache;
}

// Function to update the cache file
void updateCache(const std::string& cacheFileName, int query, const std::string& response) {
    std::ofstream cacheFile(cacheFileName, std::ios::app);
    if (cacheFile.is_open()) {
        cacheFile << query << " " << response << std::endl;
        cacheFile.close();
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    std::string query;

    // Generate unique cache file for each client based on PID
    std::string cacheFileName = getCacheFileName();
    std::cout << "Using cache file: " << cacheFileName << std::endl;

    // Load cached responses from file
    std::unordered_map<int, std::string> cache = loadCache(cacheFileName);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // Enable broadcast
    int broadcastPermission = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0) {
        std::cerr << "Error enabling broadcast." << std::endl;
        close(sockfd);
        return -1;
    }

    // Set up the broadcast address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("255.255.255.255");  // Broadcast address
    serverAddr.sin_port = htons(PORT);

    while (true) {
        std::cout << "Enter query (1:Police, 2:Ambulance, 3:Fire, 4:Vehicle, 5:Food, 6:Blood): ";
        std::getline(std::cin, query);
        int queryInt = std::stoi(query);

        // Check cache first
        if (cache.find(queryInt) != cache.end()) {
            std::cout << "Cached response: " << cache[queryInt] << std::endl;
            continue;
        }

        // Send query to the emergency server
        sendto(sockfd, query.c_str(), query.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        // Receive response from the server
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
        if (n < 0) {
            std::cerr << "Failed to receive response from server." << std::endl;
        } else {
            std::string response(buffer);
            std::cout << "Response from server: " << response << std::endl;

            // Update cache with the new response
            cache[queryInt] = response;
            updateCache(cacheFileName, queryInt, response);
        }
    }

    close(sockfd);
    return 0;
}

