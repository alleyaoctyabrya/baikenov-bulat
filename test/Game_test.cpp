
#define private public
#define protected public
#include "Game.h"
#undef private
#undef protected

#include <gtest/gtest.h>

TEST(GameTest, ConstructorInitializesDefaults) {
  Game g;

  EXPECT_EQ(g.state, GameState::MainMenu);
  EXPECT_EQ(g.mode, GameMode::Single);
  EXPECT_FALSE(g.isGameOver);
  EXPECT_FALSE(g.waitingForNetworkResult);

  EXPECT_GT(g.boardOffsetX2, g.boardOffsetX1);
  EXPECT_GT(g.boardOffsetY, 0);

  g.window.close();
}

TEST(GameTest, StartSingleModeSetsPlacingState) {
  Game g;
  g.startGame(GameMode::Single);

  EXPECT_EQ(g.state, GameState::Placing);
  EXPECT_EQ(g.mode, GameMode::Single);

  EXPECT_TRUE(g.player1.getBoard().ships.empty());

  EXPECT_EQ(g.playerAI.getBoard().ships.size(), 5u);

  g.window.close();
}

TEST(GameTest, StartHotseatModeClearsBothBoards) {
  Game g;
  g.startGame(GameMode::Hotseat);

  EXPECT_EQ(g.state, GameState::Placing);
  EXPECT_EQ(g.mode, GameMode::Hotseat);
  EXPECT_EQ(g.currentPlayerIndex, 0);
  EXPECT_TRUE(g.player1.getBoard().ships.empty());
  EXPECT_TRUE(g.player2.getBoard().ships.empty());

  g.window.close();
}

TEST(GameTest, StartNetworkHostEntersConnectingState) {
  Game g;
  g.startGame(GameMode::NetworkHost);

  EXPECT_EQ(g.mode, GameMode::NetworkHost);
  EXPECT_EQ(g.state, GameState::Connecting);
  EXPECT_FALSE(g.network.isConnected());
  EXPECT_FALSE(g.opponentReady);

  g.window.close();
}

TEST(GameTest, StartNetworkClientEndsInValidState) {
  Game g;
  g.startGame(GameMode::NetworkClient);

  ASSERT_TRUE(g.state == GameState::Placing || g.state == GameState::MainMenu);
  EXPECT_EQ(g.mode, GameMode::NetworkClient);

  g.window.close();
}
