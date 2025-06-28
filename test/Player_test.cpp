#include "Player.h"

#include <gtest/gtest.h>

TEST(PlayerTest, NameSetterGetter) {
  Player p;
  p.setName("Alice");
  EXPECT_EQ(p.getName(), "Alice");
}

TEST(PlayerTest, BoardCreatedWithShips) {
  Player p;
  EXPECT_EQ(p.getBoard().getShipsRemaining(), 5);
}
