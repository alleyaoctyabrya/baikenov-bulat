#ifndef SHIP_H
#define SHIP_H

#include <vector>

struct Coord {
  int x;
  int y;
};

enum class Orientation { Horizontal, Vertical };

class Ship {
 public:
  Ship(int size, Orientation orientation);

  void addCoord(int x, int y);

  bool occupies(int x, int y) const;

  void hit();

  bool isSunk() const;

  int getSize() const;

  const std::vector<Coord>& getCells() const;

 private:
  int size;
  Orientation orientation;
  int hits;
  std::vector<Coord> cells;
};

#endif
