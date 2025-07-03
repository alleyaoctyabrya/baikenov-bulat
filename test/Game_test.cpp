#include "Game.h"
#include <gtest/gtest.h>

TEST(GameTest, ConstructorInitializesDefaults) {
  Game g;

  // Initial state should be MainMenu and default mode Single
  EXPECT_EQ(g.getCurrentStateType(), GameStateType::MainMenu);
  EXPECT_EQ(g.getGameMode(), GameMode::Single);

  // Close the window to clean up
  g.getWindow().close();
}

TEST(GameTest, StartSingleModeSetsPlacingState) {
  Game g;
  g.startGame(GameMode::Single);

  // After starting single-player game, state should be Placing and mode Single
  EXPECT_EQ(g.getCurrentStateType(), GameStateType::Placing);
  EXPECT_EQ(g.getGameMode(), GameMode::Single);

  // Player 1's board should be empty (no ships placed yet)
  EXPECT_TRUE(g.getPlayer1().getBoard().ships.empty());
  // AI's board should have 5 ships (automatically placed for single-player mode)
  EXPECT_EQ(g.getAI().getBoard().ships.size(), 5u);

  g.getWindow().close();
}

TEST(GameTest, StartHotseatModeClearsBothBoards) {
  Game g;
  g.startGame(GameMode::Hotseat);

  // After starting hotseat game, state Placing and mode Hotseat
  EXPECT_EQ(g.getCurrentStateType(), GameStateType::Placing);
  EXPECT_EQ(g.getGameMode(), GameMode::Hotseat);
  // The first player index should be 0
  EXPECT_EQ(g.getCurrentPlayerIndex(), 0);
  // Both player boards should be cleared (no ships yet)
  EXPECT_TRUE(g.getPlayer1().getBoard().ships.empty());
  EXPECT_TRUE(g.getPlayer2().getBoard().ships.empty());

  g.getWindow().close();
}

TEST(GameTest, StartNetworkHostEntersConnectingState) {
  Game g;
  g.startGame(GameMode::NetworkHost);

  // After starting network host, mode should be NetworkHost and state should be Connecting
  EXPECT_EQ(g.getGameMode(), GameMode::NetworkHost);
  EXPECT_EQ(g.getCurrentStateType(), GameStateType::Connecting);
  // Network should not yet be connected, and opponentReady should be false
  EXPECT_FALSE(g.getNetwork().isConnected());
  EXPECT_FALSE(g.isOpponentReady());

  g.getWindow().close();
}

TEST(GameTest, StartNetworkClientEndsInValidState) {
  Game g;
  g.startGame(GameMode::NetworkClient);

  // Network client may either go to Placing or back to MainMenu (depending on implementation)
  GameStateType st = g.getCurrentStateType();
  ASSERT_TRUE(st == GameStateType::Placing || st == GameStateType::MainMenu);
  EXPECT_EQ(g.getGameMode(), GameMode::NetworkClient);

  g.getWindow().close();
}
