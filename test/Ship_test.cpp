#include "Ship.h"

#include <gtest/gtest.h>

TEST(ShipTest, InitiallyNotSunk) {
  Ship s(3, Orientation::Horizontal);
  EXPECT_FALSE(s.isSunk());
  EXPECT_EQ(s.getSize(), 3);
}

TEST(ShipTest, HitAndSunk) {
  Ship s(2, Orientation::Vertical);
  s.hit();
  EXPECT_FALSE(s.isSunk());
  s.hit();
  EXPECT_TRUE(s.isSunk());
}

TEST(ShipTest, OccupiesCoordinates) {
  Ship s(2, Orientation::Horizontal);
  s.addCoord(1, 2);
  s.addCoord(2, 2);

  EXPECT_TRUE(s.occupies(1, 2));
  EXPECT_FALSE(s.occupies(0, 0));
}
