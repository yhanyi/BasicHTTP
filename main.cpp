#include "server.hpp"

int main() {
  Server server = Server("0.0.0.0", 8080);
  server.startlisten();
  return 0;
}
