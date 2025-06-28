#include "Board.h"

#include <cstdlib>
#include <ctime>

static const int SHIP_SIZES[] = {5, 4, 3, 3, 2};
static const int NUM_SHIPS = 5;

Board::Board() {
  static bool seeded = false;
  if (!seeded) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    seeded = true;
  }
  reset(true);
}

void Board::reset(bool randomShips) {
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      shipGrid[i][j] = -1;
      hitGrid[i][j] = false;
    }
  }
  ships.clear();
  if (randomShips) {
    placeShipsRandom();
  }
}
void Board::placeShipsRandom() {
  int taken[SIZE][SIZE] = {{0}};
  for (int s = 0; s < NUM_SHIPS; ++s) {
    int size = SHIP_SIZES[s];
    bool placed = false;
    while (!placed) {
      Orientation orient = (std::rand() % 2 == 0 ? Orientation::Horizontal
                                                 : Orientation::Vertical);
      int x = std::rand() % SIZE;
      int y = std::rand() % SIZE;
      bool fit = true;
      if (orient == Orientation::Horizontal) {
        if (x + size > SIZE)
          fit = false;
        else {
          for (int ix = x; ix < x + size; ++ix) {
            if (taken[y][ix] != 0) {
              fit = false;
              break;
            }
          }
        }
      } else {
        if (y + size > SIZE)
          fit = false;
        else {
          for (int iy = y; iy < y + size; ++iy) {
            if (taken[iy][x] != 0) {
              fit = false;
              break;
            }
          }
        }
      }
      if (!fit) continue;

      Ship ship(size, orient);
      if (orient == Orientation::Horizontal) {
        for (int ix = x; ix < x + size; ++ix) {
          ship.addCoord(ix, y);
          shipGrid[y][ix] = ships.size();
        }

        for (int ix = x - 1; ix <= x + size; ++ix) {
          for (int iy = y - 1; iy <= y + 1; ++iy) {
            if (ix >= 0 && ix < SIZE && iy >= 0 && iy < SIZE) {
              taken[iy][ix] = 1;
            }
          }
        }
      } else {
        for (int iy = y; iy < y + size; ++iy) {
          ship.addCoord(x, iy);
          shipGrid[iy][x] = ships.size();
        }

        for (int iy = y - 1; iy <= y + size; ++iy) {
          for (int ix = x - 1; ix <= x + 1; ++ix) {
            if (ix >= 0 && ix < SIZE && iy >= 0 && iy < SIZE) {
              taken[iy][ix] = 1;
            }
          }
        }
      }
      ships.push_back(ship);
      placed = true;
    }
  }
}

bool Board::allShipsSunk() const {
  for (const Ship& ship : ships) {
    if (!ship.isSunk()) {
      return false;
    }
  }
  return true;
}

int Board::getShipsRemaining() const {
  int count = 0;
  for (const Ship& ship : ships) {
    if (!ship.isSunk()) {
      count++;
    }
  }
  return count;
}

ShotResult Board::receiveShot(int x, int y) {
  if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
    return ShotResult::Miss;
  }
  if (hitGrid[y][x]) {
    return ShotResult::Miss;
  }
  hitGrid[y][x] = true;
  if (shipGrid[y][x] == -1) {
    return ShotResult::Miss;
  } else {
    int shipIndex = shipGrid[y][x];
    Ship& ship = ships[shipIndex];
    ship.hit();
    if (ship.isSunk()) {
      markSurroundings(shipIndex);

      if (allShipsSunk()) {
        return ShotResult::AllSunk;
      } else {
        return ShotResult::Sunk;
      }
    } else {
      return ShotResult::Hit;
    }
  }
}

void Board::markSurroundings(int shipIndex) {
  Ship& ship = ships[shipIndex];
  for (const Coord& c : ship.getCells()) {
    int cx = c.x;
    int cy = c.y;
    for (int ny = cy - 1; ny <= cy + 1; ++ny) {
      for (int nx = cx - 1; nx <= cx + 1; ++nx) {
        if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
          if (!hitGrid[ny][nx] && shipGrid[ny][nx] == -1) {
            hitGrid[ny][nx] = true;
          }
        }
      }
    }
  }
}

void Board::draw(sf::RenderWindow& window, int offsetX, int offsetY,
                 bool showShips, const sf::Texture* texShip,
                 const sf::Texture* texHit, const sf::Texture* texMiss) {
  sf::RectangleShape line;
  line.setFillColor({50, 50, 50});
  for (int i = 0; i <= SIZE; ++i) {
    line.setSize({1.f, SIZE * CELL_SIZE});
    line.setPosition(offsetX + i * CELL_SIZE, offsetY);
    window.draw(line);

    line.setSize({SIZE * CELL_SIZE, 1.f});
    line.setPosition(offsetX, offsetY + i * CELL_SIZE);
    window.draw(line);
  }

  sf::Sprite sprShip, sprHit, sprMiss;
  if (texShip) {
    sprShip.setTexture(*texShip);
  }
  if (texHit) {
    sprHit.setTexture(*texHit);
  }
  if (texMiss) {
    sprMiss.setTexture(*texMiss);
  }

  auto fitSprite = [](sf::Sprite& s) {
    auto sz = s.getTexture()->getSize();
    float scale = CELL_SIZE / static_cast<float>(std::max(sz.x, sz.y));
    s.setScale(scale, scale);
  };
  if (texShip) fitSprite(sprShip);
  if (texHit) fitSprite(sprHit);
  if (texMiss) fitSprite(sprMiss);

  sf::RectangleShape cell({(float)CELL_SIZE, (float)CELL_SIZE});
  sf::CircleShape dot(4.f);
  dot.setFillColor(sf::Color::Black);

  for (int y = 0; y < SIZE; ++y)
    for (int x = 0; x < SIZE; ++x) {
      float px = offsetX + x * CELL_SIZE;
      float py = offsetY + y * CELL_SIZE;

      if (showShips && shipGrid[y][x] != -1) {
        if (texShip) {
          sprShip.setPosition(px, py);
          window.draw(sprShip);
        } else {
          cell.setPosition(px, py);
          cell.setFillColor({100, 100, 100});
          window.draw(cell);
        }
      }

      if (hitGrid[y][x]) {
        if (shipGrid[y][x] != -1) {
          if (texHit) {
            sprHit.setPosition(px, py);
            window.draw(sprHit);
          } else {
            sf::Vertex xLines[4] = {
                {{px + 4, py + 4}, sf::Color::Red},
                {{px + CELL_SIZE - 4, py + CELL_SIZE - 4}, sf::Color::Red},
                {{px + CELL_SIZE - 4, py + 4}, sf::Color::Red},
                {{px + 4, py + CELL_SIZE - 4}, sf::Color::Red}};
            window.draw(xLines, 4, sf::Lines);
          }
        } else {
          if (texMiss) {
            sprMiss.setPosition(px, py);
            window.draw(sprMiss);
          } else {
            dot.setPosition(px + CELL_SIZE / 2 - 4, py + CELL_SIZE / 2 - 4);
            window.draw(dot);
          }
        }
      }
    }
}
bool Board::wasShot(int x, int y) const {
  if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return false;
  return hitGrid[y][x];
}
