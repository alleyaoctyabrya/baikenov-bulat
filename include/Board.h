#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "Ship.h"
const int CELL_SIZE = 32;

enum class ShotResult { Miss, Hit, Sunk, AllSunk };

class Game;

class Board {
 public:
  Board();

  void reset(bool randomShips = true);

  void placeShipsRandom();

  bool allShipsSunk() const;

  int getShipsRemaining() const;

  ShotResult receiveShot(int x, int y);

  void draw(sf::RenderWindow& window, int offsetX, int offsetY, bool showShips,
            const sf::Texture* texShip = nullptr,
            const sf::Texture* texHit = nullptr,
            const sf::Texture* texMiss = nullptr);

  bool wasShot(int x, int y) const;

  friend class Game;

  static const int SIZE = 10;

  void markSurroundings(int shipIndex);

  int shipGrid[SIZE][SIZE];

  bool hitGrid[SIZE][SIZE];

  std::vector<Ship> ships;
};

#endif
