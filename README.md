# Emergency Service System

## Overview

The Emergency Service System is a UDP-based client-server application designed to provide quick access to emergency service contact numbers. It consists of a server that holds emergency service information and multiple clients that can query this information.

## Features

- UDP-based communication for fast, lightweight queries
- Server supports multiple concurrent clients
- Client-side caching to reduce network load and improve response times
- Broadcast messaging for easy server discovery
- Support for multiple emergency services (Police, Ambulance, Fire, Vehicle Repair, Food Delivery, Blood Bank)

## Requirements

- C++ compiler with C++11 support
- POSIX-compliant operating system (Linux, macOS, etc.)
- pthread library

## Building the Project

To compile the server and client, use the following commands:

```bash
g++ -std=c++11 -pthread server.cpp -o server
g++ -std=c++11 client.cpp -o client
```

## Running the Application

1. Start the server:

   ```
   ./server
   ```

2. In separate terminal windows, run one or more clients:
   ```
   ./client
   ```

## Usage

### Server

The server automatically starts listening for client queries on port 8080. No additional input is required.

### Client

1. The client will prompt you to enter a query number:

   ```
   Enter query (1:Police, 2:Ambulance, 3:Fire, 4:Vehicle, 5:Food, 6:Blood):
   ```

2. Enter a number between 1 and 6 to query the corresponding emergency service.

3. The client will display the response, either from its local cache or from the server.

4. Repeat step 1 to make additional queries.

## Protocol Details

- Communication: UDP on port 8080
- Query format: Single integer (1-6)
- Response format: String containing service name and phone number
- Maximum message size: 1024 bytes

## File Structure

- `server.cpp`: Implementation of the UDP server
- `client.cpp`: Implementation of the UDP client
- `README.md`: This file, containing project documentation

## Caching

Each client maintains a local cache file named `cache_[PID].txt`, where [PID] is the process ID of the client. This file stores previously received responses to reduce network usage and improve response times for repeated queries.

## Security Note

This application uses UDP broadcast messages, which are not encrypted. For use in a production environment, additional security measures should be implemented.

## DORA Process

W Our Emergency Service System follows the DHCP DORA (Discover, Offer, Request, Acknowledgment) process, we can draw an analogy to help understand the communication flow. Let's break it down with code examples:

### Actual Implementation

1. **Query (combines Discover and Request):**
   The client sends a query to the server using UDP broadcast.

   ```cpp:client.cpp
   // Client sends query
   std::cout << "Enter query (1:Police, 2:Ambulance, 3:Fire, 4:Vehicle, 5:Food, 6:Blood): ";
   std::getline(std::cin, query);
   int queryInt = std::stoi(query);

   sendto(sockfd, query.c_str(), query.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
   ```

2. **Response (combines Offer and Acknowledge):**
   The server processes the query and sends back the response.

   ```cpp:server.cpp
   // Server receives query and sends response
   int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
   int query = atoi(buffer);

   std::string response = "Invalid query";
   if (emergencyNumbers.find(query) != emergencyNumbers.end()) {
       response = emergencyNumbers[query];
   }

   sendto(sock, response.c_str(), response.size(), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
   ```

### DHCP-like Analogy

To humor the analogy, let's break it down in a DORA-like way:

1. **Discover:**
   The client sends out a "discover" message.

   ```cpp:client.cpp
   // Client "discovers" by sending query
   std::string query = "1";  // Asking for Police number
   sendto(sockfd, query.c_str(), query.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
   ```

2. **Offer:**
   The server responds with an "offer".

   ```cpp:server.cpp
   // Server "offers" by processing query
   int query = atoi(buffer);
   std::string response = emergencyNumbers[query];
   // (Preparing to send, actual send in Acknowledge step)
   ```

3. **Request:**
   In our simplified system, the initial query acts as both Discover and Request.

4. **Acknowledge:**
   The server "acknowledges" by sending the response.

   ```cpp:server.cpp
   // Server "acknowledges" by sending response
   sendto(sock, response.c_str(), response.size(), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
   ```

   The client receives the acknowledgment:

   ```cpp:client.cpp
   // Client receives "acknowledgment"
   memset(buffer, 0, BUFFER_SIZE);
   int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
   if (n < 0) {
       std::cerr << "Failed to receive response from server." << std::endl;
   } else {
       std::string response(buffer);
       std::cout << "Response from server: " << response << std::endl;
   }
   ```

### Key Differences from Traditional DORA

1. **Simplified Process:** Our system combines the traditional four steps into essentially two steps: query (combining Discover and Request) and response (combining Offer and Acknowledgment).
2. **Stateless Communication:** Unlike DHCP, our system uses stateless UDP communication. Each query-response pair is independent.
3. **Broadcast Usage:** The client uses broadcast for all communications, not just discovery.
4. **No Lease Time:** The information provided by the server doesn't have an expiration time, though the client does cache responses.

This simplified DORA-like process allows for quick, efficient communication of emergency service information in a lightweight, easy-to-implement system, while still maintaining some conceptual similarities to the more complex DHCP DORA process.
