#pragma once

#include <functional>
#include <netinet/in.h>
#include <string>
#include <unordered_map>

class Server {
public:
  Server(std::string ipAddress, int port);
  ~Server();
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  void start();

  void registerHandler(const std::string &path,
                       std::function<std::string(void)> handler);

private:
  void acceptConnection();
  void handleConnection();
  void sendResponse(const std::string &content);
  void closeServer();
  std::string parseRequestPath(const std::string &path);

  int serverFd;
  int clientFd;
  const int port;
  sockaddr_in socketAddress;
  socklen_t socketLength;

  static constexpr int MAX_BUFFER = 4096;
  static constexpr int MAX_CONNECTIONS = 10;

  std::unordered_map<std::string, std::function<std::string(void)>>
      pathHandlers;
  bool isRunning;
};
