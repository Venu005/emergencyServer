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
g++  server.cpp -o server
g++  client.cpp -o client
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
