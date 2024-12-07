#pragma once

#include <netinet/in.h>
#include <string>

class Server {
public:
  Server(std::string, int);
  ~Server();
  int start();
  void closeserver();
  void startlisten();
  void acceptconn();
  void sendresponse();

private:
  int sfd;
  int cfd;
  int port;
  sockaddr_in socketAddress;
  socklen_t socketLength;
};
