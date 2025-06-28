#include "AI.h"

#include <gtest/gtest.h>

TEST(AITest, MakesLegalMove) {
  AI ai;
  Board enemy;

  int before = 0;
  for (int y = 0; y < Board::SIZE; ++y)
    for (int x = 0; x < Board::SIZE; ++x) before += enemy.hitGrid[y][x] ? 1 : 0;

  ai.makeMove(enemy);

  int after = 0;
  for (int y = 0; y < Board::SIZE; ++y)
    for (int x = 0; x < Board::SIZE; ++x) after += enemy.hitGrid[y][x] ? 1 : 0;

  EXPECT_EQ(before + 1, after);
}
