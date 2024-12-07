#include "server.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace {
void log(const std::string &message) {
  std::cout << "[SERVER] " << message << std::endl;
}

std::string formatSocketInfo(const sockaddr_in &addr) {
  return std::string("\n[IP Address] ") + inet_ntoa(addr.sin_addr) +
         "\n[PORT] " + std::to_string(ntohs(addr.sin_port));
}
} // namespace

Server::Server(std::string ipAddress, int port) : port(port), isRunning(false) {
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = htons(port);
  socketAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
  socketLength = sizeof(socketAddress);

  registerHandler("/", []() {
    return "<html><body><h1>Welcome to Basic HTTP Server</h1></body></html>";
  });

  serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverFd < 0) {
    throw std::runtime_error("Socket creation failed.");
  }

  int reuse = 1;
  if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    close(serverFd);
    throw std::runtime_error("Failed to set socket options.");
  }

  if (bind(serverFd, reinterpret_cast<struct sockaddr *>(&socketAddress),
           socketLength) < 0) {
    close(serverFd);
    throw std::runtime_error("Socket binding failed");
  }

  log("Server initialised successfully." + formatSocketInfo(socketAddress));
}

Server::~Server() { closeServer(); }

void Server::registerHandler(const std::string &path,
                             std::function<std::string(void)> handler) {
  pathHandlers[path] = std::move(handler);
}

void Server::start() {
  if (listen(serverFd, MAX_CONNECTIONS) < 0) {
    throw std::runtime_error("Failed to start listening.");
  }

  log("Server is listening." + formatSocketInfo(socketAddress));
  isRunning = true;

  while (isRunning) {
    acceptConnection();
    handleConnection();
    close(clientFd);
  }
}

void Server::acceptConnection() {
  clientFd =
      accept(serverFd, reinterpret_cast<struct sockaddr *>(&socketAddress),
             &socketLength);
  if (clientFd < 0) {
    throw std::runtime_error("Failed to accept client connection.");
  }
}

std::string Server::parseRequestPath(const std::string &request) {
  std::istringstream iss(request);
  std::string method, path, protocol;
  iss >> method >> path >> protocol;
  return path;
}

void Server::handleConnection() {
  char buffer[MAX_BUFFER] = {0};
  ssize_t bytesRead = read(clientFd, buffer, MAX_BUFFER - 1);

  if (bytesRead < 0) {
    throw std::runtime_error("Error reading from client");
  }

  std::string request(buffer);
  std::string path = parseRequestPath(request);

  std::string content;
  if (pathHandlers.count(path) > 0) {
    content = pathHandlers[path]();
  } else {
    content = "<html><body><h1>404 Not Found</h1></body></html>";
  }

  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: " +
                         std::to_string(content.length()) +
                         "\r\n"
                         "\r\n" +
                         content;

  sendResponse(response);
}

void Server::sendResponse(const std::string &response) {
  ssize_t bytesSent = write(clientFd, response.c_str(), response.length());
  if (bytesSent != static_cast<ssize_t>(response.length())) {
    throw std::runtime_error("Failed to send complete response.");
  }
  log("Response sent successfully.");
}

void Server::closeServer() {
  isRunning = false;
  if (serverFd >= 0)
    close(serverFd);
  if (clientFd >= 0)
    close(clientFd);
}
