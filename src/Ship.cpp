#include "Ship.h"

Ship::Ship(int size, Orientation orientation)
    : size(size), orientation(orientation), hits(0) {
  cells.reserve(size);
}

void Ship::addCoord(int x, int y) { cells.push_back({x, y}); }

bool Ship::occupies(int x, int y) const {
  for (const Coord& c : cells) {
    if (c.x == x && c.y == y) {
      return true;
    }
  }
  return false;
}

void Ship::hit() { hits++; }

bool Ship::isSunk() const { return hits >= size; }

int Ship::getSize() const { return size; }

const std::vector<Coord>& Ship::getCells() const { return cells; }
