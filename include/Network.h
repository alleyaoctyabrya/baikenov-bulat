#ifndef NETWORK_H
#define NETWORK_H

#include <SFML/Network.hpp>

class Network {
 public:
  Network();
  ~Network();

  bool startServer(unsigned short port);

  bool acceptClient();

  bool connectToServer(const sf::IpAddress& ip, unsigned short port);

  bool sendShot(int x, int y);

  bool receiveShot(int& x, int& y);

  bool sendResult(int resultCode);

  bool receiveResult(int& resultCode);

  bool isConnected() const;

  void disconnect();

 private:
  sf::TcpListener listener;
  sf::TcpSocket socket;
  bool connected;
  bool isServer;
};

#endif
