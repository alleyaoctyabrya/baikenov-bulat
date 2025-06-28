#include "Board.h"

#include <gtest/gtest.h>

TEST(BoardTest, ShipsPlacedRandomly) {
  Board b;
  EXPECT_EQ(b.getShipsRemaining(), 5);
}

TEST(BoardTest, ReceiveShotMiss) {
  Board b;

  bool checked = false;
  for (int y = 0; y < Board::SIZE && !checked; ++y)
    for (int x = 0; x < Board::SIZE && !checked; ++x)
      if (b.shipGrid[y][x] == -1) {
        EXPECT_EQ(b.receiveShot(x, y), ShotResult::Miss);
        EXPECT_TRUE(b.wasShot(x, y));
        checked = true;
      }
  EXPECT_TRUE(checked);
}

TEST(BoardTest, SinkAllShips) {
  Board b;
  for (const Ship& s : b.ships)
    for (const Coord& c : s.getCells()) b.receiveShot(c.x, c.y);

  EXPECT_TRUE(b.allShipsSunk());
}
