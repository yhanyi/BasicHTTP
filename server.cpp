#include "server.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_BUFFER = 1024;
const int MAX_CONN = 20;
void log(const std::string &message) { std::cout << message << std::endl; }
std::string logSocket(sockaddr_in &s) {
  return "\n[IP Address] " + std::string(inet_ntoa(s.sin_addr)) + '\n' +
         "[PORT] " + std::to_string(ntohs(s.sin_port));
}
void errorExit(const std::string &error) {
  log(error);
  exit(1);
}

Server::Server(std::string ipAddress, int port) : port(port), socketAddress() {
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = htons(port);
  socketAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
  socketLength = sizeof(socketAddress);

  std::ostringstream oss;

  if (!start()) {
    oss << "Server failed to start." << logSocket(socketAddress);
    log(oss.str());
    return;
  }

  oss << "Server started successfully." << logSocket(socketAddress);
  log(oss.str());
}

Server::~Server() { closeserver(); }

int Server::start() {
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    errorExit("Socket creation failed.");
    return 1;
  }
  int reuse = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) <
      0) {
    errorExit("Socket options failed.");
    return 1;
  }
  if (bind(sfd, (struct sockaddr *)&socketAddress, socketLength) < 0) {
    errorExit("Socket binding failed.");
    return 1;
  }
  return 0;
}

void Server::startlisten() {
  std::ostringstream oss;
  if (listen(sfd, MAX_CONN) < 0) {
    oss << "Server failed to listen." << logSocket(socketAddress);
    errorExit(oss.str());
  }
  oss << "Server is listening." << logSocket(socketAddress);
  log(oss.str());

  while (1) {
    acceptconn();
    char buffer[MAX_BUFFER] = {0};
    int bytes = read(cfd, buffer, MAX_BUFFER);

    std::ostringstream ss;

    if (bytes < 0) {
      oss << "Error encountered during reading.";
      errorExit(oss.str());
    }

    {
      oss << "Client read successfully.";
      log(oss.str());
    }

    sendresponse();
    close(cfd);
  }
}

void Server::acceptconn() {
  cfd = accept(sfd, (struct sockaddr *)&socketAddress,
               (socklen_t *)&socketLength);
  if (cfd < 0) {
    std::ostringstream oss;
    oss << "Error encountered in accepting client connection.";
    errorExit(oss.str());
  }
}

void Server::sendresponse() {
  std::string response = "HTTP/1.1 200 OK\r\n\r\n";
  long bytesSent = write(cfd, response.c_str(), response.size());
  std::ostringstream oss;
  if (bytesSent == response.size()) {
    oss << "Wrote to client successfully.";
    log(oss.str());
  } else {
    oss << "Error writing to client";
    errorExit(oss.str());
  }
}

void Server::closeserver() {
  close(sfd);
  close(cfd);
}
