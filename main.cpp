#include "server.hpp"
#include <iostream>

int main() {
  try {
    Server server("0.0.0.0", 8080);

    server.registerHandler("/hello", []() {
      return "<html><body><h1>Hello, World!</h1></body></html>";
    });

    server.registerHandler("/about", []() {
      return "<html><body><h1>About Page</h1><p>This is a basic HTTP "
             "server.</p></body></html>";
    });

    server.start();
  } catch (const std::exception &e) {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
