# HTTP Server

A basic HTTP server implementation in C++ that supports custom route handlers and basic HTTP responses.

## Build Instructions

```bash
# Create build directory and navigate into it
mkdir -p build && cd build

# Generate build files
cmake ..

# Build the project
cmake --build .
```

## Testing the Server

Start the server:

```bash
./BasicHTTP
```

Test with various HTTP clients:

```bash
# Using curl
curl -i http://localhost:8080         # Root path
curl -i http://localhost:8080/hello   # Hello page
curl -i http://localhost:8080/about   # About page

# Using telnet for raw HTTP requests
telnet localhost 8080
# Then type:
GET / HTTP/1.1
Host: localhost

# Press Enter twice
```

## Technical Implementation Details

### 1. Socket Creation and Server Setup
The server initialization process follows these key steps:

1. **Socket Creation**

   ```cpp
   serverFd = socket(AF_INET, SOCK_STREAM, 0);
   ```
   - Creates a TCP socket using IPv4
   - `AF_INET`: IPv4 Internet protocols
   - `SOCK_STREAM`: TCP socket type
   - Returns a file descriptor for the socket

2. **Socket Options**

   ```cpp
   setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
   ```
   - Configures socket to reuse the address
   - Prevents "Address already in use" errors
   - Useful during development and quick server restarts

3. **Binding**

   ```cpp
   bind(serverFd, (struct sockaddr*)&socketAddress, socketLength);
   ```
   - Binds socket to specific IP address and port
   - `socketAddress` contains:
     - IP address (e.g., "0.0.0.0" for all interfaces)
     - Port number (e.g., 8080)
     - Address family (AF_INET)

4. **Listening**

   ```cpp
   listen(serverFd, MAX_CONNECTIONS);
   ```
   - Marks socket as passive for accepting connections
   - `MAX_CONNECTIONS` defines maximum pending connections queue length

### 2. Client Connection Handling

1. **Accept Connection**

   ```cpp
   clientFd = accept(serverFd, (struct sockaddr*)&socketAddress, &socketLength);
   ```
   - Accepts pending connection from queue
   - Returns new socket descriptor for this specific client
   - Blocks until client connects

2. **Read Request**

   ```cpp
   read(clientFd, buffer, MAX_BUFFER);
   ```
   - Reads HTTP request from client
   - Stores data in buffer
   - MAX_BUFFER (4096 bytes) defines maximum request size

3. **Process Request**

   - Parse HTTP request to extract:
     - Method (GET, POST, etc.)
     - Path (/hello, /about, etc.)
     - Headers
   - Route to appropriate handler based on path

4. **Send Response**

   ```cpp
   write(clientFd, response.c_str(), response.length());
   ```
   - Sends HTTP response back to client
   - Includes status line, headers, and body
   - Format:
     ```
     HTTP/1.1 200 OK
     Content-Type: text/html
     Content-Length: <length>

     <html content>
     ```

5. **Cleanup**

   ```cpp
   close(clientFd);
   ```
   - Closes client connection after response
   - Frees system resources

### Route Handler System

The server implements a simple routing system:

```cpp
std::unordered_map<std::string, std::function<std::string(void)>> pathHandlers;
```

Register routes with custom handlers:
```cpp
server.registerHandler("/hello", []() {
    return "<html><body><h1>Hello, World!</h1></body></html>";
});
```

### Resource Management

- RAII principles ensure proper resource cleanup
- Server uses exception handling for error cases
- Socket descriptors automatically closed in destructor
- Copy constructor/assignment operator deleted to prevent resource leaks
