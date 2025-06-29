#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Ship.h"

constexpr int CELL_SIZE = 32;

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

  static constexpr int SIZE = 10;

  int shipGrid[SIZE][SIZE];
  bool hitGrid[SIZE][SIZE];
  std::vector<Ship> ships;

 private:
  bool canPlaceShip(int x, int y, int size, Orientation orient,
                    const int (&taken)[SIZE][SIZE]) const;
  void commitShip(const Ship& ship);
  void markAdjacentTaken(int x, int y, int size, Orientation orient,
                         int (&taken)[SIZE][SIZE]);

  void drawGrid(sf::RenderWindow& window, int offsetX, int offsetY) const;
  void drawCellSprites(sf::RenderWindow& window, int x, int y, int offsetX,
                       int offsetY, bool showShips, const sf::Texture* texShip,
                       const sf::Texture* texHit,
                       const sf::Texture* texMiss) const;

  void markSurroundings(int shipIndex);
  bool isShip(int x, int y) const;
  void drawShip(sf::RenderWindow& win, float px, float py,
                const sf::Texture* texShip) const;
  void drawHit(sf::RenderWindow& win, float px, float py,
               const sf::Texture* texHit) const;
  void drawMiss(sf::RenderWindow& win, float px, float py,
                const sf::Texture* texMiss) const;
};

#endif
