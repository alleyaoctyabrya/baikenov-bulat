#include "Network.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

TEST(NetworkTest, ConnectAndExchange) {
  const unsigned short port = 62000;

  Network server;
  ASSERT_TRUE(server.startServer(port));

  std::thread acceptThread([&] {
    for (int i = 0; i < 50 && !server.isConnected(); ++i) {
      server.acceptClient();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  Network client;
  ASSERT_TRUE(client.connectToServer(sf::IpAddress::LocalHost, port));

  acceptThread.join();
  ASSERT_TRUE(server.isConnected());
  ASSERT_TRUE(client.isConnected());

  EXPECT_TRUE(client.sendShot(3, 4));

  int x = -1, y = -1;
  for (int i = 0; i < 50 && !server.receiveShot(x, y); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_EQ(std::make_pair(x, y), std::make_pair(3, 4));

  EXPECT_TRUE(server.sendResult(1));

  int code = -1;
  for (int i = 0; i < 50 && !client.receiveResult(code); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_EQ(code, 1);

  server.disconnect();
  client.disconnect();
}
