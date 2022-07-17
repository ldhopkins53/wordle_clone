#include <cstdlib>
#include <iostream>

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>

#include "wordle.hpp"

void validate_arguments(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " PORT WORDLIST" << std::endl;
    std::exit(1);
  }
}

int main(int argc, char **argv) {
  validate_arguments(argc, argv);
  const auto target_port = std::stoi(argv[1]);
  const std::string wordlist{argv[2]};

  Poco::Net::ServerSocket srv(target_port); // does bind + listen
  std::cout << "[+] Creating server on " << target_port << std::endl;
  while (true) {
    Poco::Net::StreamSocket ss = srv.acceptConnection();
    auto addr = ss.peerAddress();
    std::cout << "[+] Connection accepted from " << addr.host() << ":" << addr.port() << std::endl;
    Poco::Net::SocketStream str(ss);
    Wordle w(str, wordlist);
    w.run();
  }
}
