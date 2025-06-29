#include "Network.h"

Network::Network() : connected(false), isServer(false) {
  socket.setBlocking(false);
}

Network::~Network() { disconnect(); }

bool Network::startServer(unsigned short port) {
  listener.close();
  socket.disconnect();
  if (listener.listen(port) != sf::Socket::Done) {
    return false;
  }
  listener.setBlocking(false);
  isServer = true;
  connected = false;
  return true;
}

bool Network::acceptClient() {
  if (!isServer) {
    return false;
  }

  if (listener.accept(socket) == sf::Socket::Done) {
    listener.close();
    socket.setBlocking(false);
    connected = true;
    return true;
  }
  return false;
}

bool Network::connectToServer(const sf::IpAddress& ip, unsigned short port) {
  isServer = false;

  socket.disconnect();
  socket.setBlocking(true);
  sf::Socket::Status status = socket.connect(ip, port, sf::seconds(5.f));
  socket.setBlocking(false);
  if (status == sf::Socket::Done) {
    connected = true;
    return true;
  } else {
    connected = false;
    socket.disconnect();
    return false;
  }
}

bool Network::sendShot(int x, int y) {
  if (!connected) {
    return false;
  }
  sf::Packet packet;
  packet << x << y;

  socket.setBlocking(true);
  sf::Socket::Status status = socket.send(packet);
  socket.setBlocking(false);
  return (status == sf::Socket::Done);
}

bool Network::receiveShot(int& x, int& y) {
  if (!connected) {
    return false;
  }
  sf::Packet packet;
  sf::Socket::Status status = socket.receive(packet);
  if (status == sf::Socket::Done) {
    if (packet.getDataSize() == sizeof(sf::Int32) * 2) {
      packet >> x >> y;
      return true;
    }
  } else if (status == sf::Socket::Disconnected) {
    connected = false;
  }
  return false;
}

bool Network::sendResult(int resultCode) {
  if (!connected) {
    return false;
  }
  sf::Packet packet;
  packet << resultCode;

  socket.setBlocking(true);
  sf::Socket::Status status = socket.send(packet);
  socket.setBlocking(false);
  return (status == sf::Socket::Done);
}

bool Network::receiveResult(int& resultCode) {
  if (!connected) {
    return false;
  }
  sf::Packet packet;
  sf::Socket::Status status = socket.receive(packet);
  if (status == sf::Socket::Done) {
    if (packet.getDataSize() == sizeof(sf::Int32)) {
      packet >> resultCode;
      return true;
    }
  } else if (status == sf::Socket::Disconnected) {
    connected = false;
  }
  return false;
}

bool Network::isConnected() const { return connected; }

void Network::disconnect() {
  connected = false;
  socket.disconnect();
  listener.close();
}
