#include "Board.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

static constexpr int SHIP_SIZES[] = {5, 4, 3, 3, 2};
static constexpr int NUM_SHIPS = 5;

Board::Board() {
  static bool seeded = false;
  if (!seeded) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    seeded = true;
  }
  reset(true);
}

void Board::reset(bool randomShips) {
  for (auto& row : shipGrid)
    std::fill(std::begin(row), std::end(row), -1);
  for (auto& row : hitGrid)
    std::fill(std::begin(row), std::end(row), false);
  ships.clear();

  if (randomShips)
    placeShipsRandom();
}

void Board::placeShipsRandom() {
  int taken[SIZE][SIZE] = {{0}};

  for (int s = 0; s < NUM_SHIPS; ++s) {
    const int size = SHIP_SIZES[s];
    bool placed = false;

    while (!placed) {
      const Orientation orient =
          std::rand() % 2 ? Orientation::Horizontal : Orientation::Vertical;
      const int x = std::rand() % SIZE;
      const int y = std::rand() % SIZE;

      if (!canPlaceShip(x, y, size, orient, taken))
        continue;

      Ship ship(size, orient);
      for (int i = 0; i < size; ++i) {
        int cx = x + (orient == Orientation::Horizontal ? i : 0);
        int cy = y + (orient == Orientation::Horizontal ? 0 : i);
        ship.addCoord(cx, cy);
      }

      commitShip(ship);
      markAdjacentTaken(x, y, size, orient, taken);
      placed = true;
    }
  }
}

bool Board::canPlaceShip(int x, int y, int size, Orientation orient,
                         const int (&taken)[SIZE][SIZE]) const {
  if (orient == Orientation::Horizontal) {
    if (x + size > SIZE)
      return false;
    for (int ix = x; ix < x + size; ++ix)
      if (taken[y][ix])
        return false;
  } else {
    if (y + size > SIZE)
      return false;
    for (int iy = y; iy < y + size; ++iy)
      if (taken[iy][x])
        return false;
  }
  return true;
}

void Board::commitShip(const Ship& ship) {
  int index = static_cast<int>(ships.size());
  ships.push_back(ship);
  for (const Coord& c : ship.getCells())
    shipGrid[c.y][c.x] = index;
}

void Board::markAdjacentTaken(int x, int y, int size, Orientation orient,
                              int (&taken)[SIZE][SIZE]) {
  if (orient == Orientation::Horizontal) {
    for (int ix = x - 1; ix <= x + size; ++ix)
      for (int iy = y - 1; iy <= y + 1; ++iy)
        if (ix >= 0 && ix < SIZE && iy >= 0 && iy < SIZE)
          taken[iy][ix] = 1;
  } else {
    for (int iy = y - 1; iy <= y + size; ++iy)
      for (int ix = x - 1; ix <= x + 1; ++ix)
        if (ix >= 0 && ix < SIZE && iy >= 0 && iy < SIZE)
          taken[iy][ix] = 1;
  }
}

bool Board::allShipsSunk() const {
  return std::all_of(ships.begin(), ships.end(),
                     [](const Ship& s) { return s.isSunk(); });
}

int Board::getShipsRemaining() const {
  return static_cast<int>(std::count_if(
      ships.begin(), ships.end(), [](const Ship& s) { return !s.isSunk(); }));
}

ShotResult Board::receiveShot(int x, int y) {
  if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
    return ShotResult::Miss;
  if (hitGrid[y][x])
    return ShotResult::Miss;

  hitGrid[y][x] = true;

  if (shipGrid[y][x] == -1)
    return ShotResult::Miss;

  int shipIdx = shipGrid[y][x];
  Ship& ship = ships[shipIdx];
  ship.hit();

  if (ship.isSunk()) {
    markSurroundings(shipIdx);
    return allShipsSunk() ? ShotResult::AllSunk : ShotResult::Sunk;
  }
  return ShotResult::Hit;
}

void Board::markSurroundings(int shipIndex) {
  Ship& ship = ships[shipIndex];
  for (const Coord& c : ship.getCells())
    for (int ny = c.y - 1; ny <= c.y + 1; ++ny)
      for (int nx = c.x - 1; nx <= c.x + 1; ++nx)
        if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE)
          if (!hitGrid[ny][nx] && shipGrid[ny][nx] == -1)
            hitGrid[ny][nx] = true;
}

bool Board::wasShot(int x, int y) const {
  return (x >= 0 && x < SIZE && y >= 0 && y < SIZE) && hitGrid[y][x];
}

void Board::draw(sf::RenderWindow& window, int offsetX, int offsetY,
                 bool showShips, const sf::Texture* texShip,
                 const sf::Texture* texHit, const sf::Texture* texMiss) {
  drawGrid(window, offsetX, offsetY);

  for (int y = 0; y < SIZE; ++y)
    for (int x = 0; x < SIZE; ++x)
      drawCellSprites(window, x, y, offsetX, offsetY, showShips, texShip,
                      texHit, texMiss);
}

void Board::drawGrid(sf::RenderWindow& window, int offsetX, int offsetY) const {
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
}

bool Board::isShip(int x, int y) const { return shipGrid[y][x] != -1; }

void Board::drawShip(sf::RenderWindow& win, float px, float py,
                     const sf::Texture* texShip) const {
  if (texShip) {
    sf::Sprite s{*texShip};
    auto sz = texShip->getSize();
    float k = CELL_SIZE / static_cast<float>(std::max(sz.x, sz.y));
    s.setScale(k, k);
    s.setPosition(px, py);
    win.draw(s);
  } else {
    sf::RectangleShape cell({(float)CELL_SIZE, (float)CELL_SIZE});
    cell.setFillColor({100, 100, 100});
    cell.setPosition(px, py);
    win.draw(cell);
  }
}

void Board::drawHit(sf::RenderWindow& win, float px, float py,
                    const sf::Texture* texHit) const {
  if (texHit) {
    sf::Sprite s{*texHit};
    auto sz = texHit->getSize();
    float k = CELL_SIZE / static_cast<float>(std::max(sz.x, sz.y));
    s.setScale(k, k);
    s.setPosition(px, py);
    win.draw(s);
  } else {
    sf::Vertex cross[4] = {
        {{px + 4, py + 4}, sf::Color::Red},
        {{px + CELL_SIZE - 4, py + CELL_SIZE - 4}, sf::Color::Red},
        {{px + CELL_SIZE - 4, py + 4}, sf::Color::Red},
        {{px + 4, py + CELL_SIZE - 4}, sf::Color::Red}};
    win.draw(cross, 4, sf::Lines);
  }
}

void Board::drawMiss(sf::RenderWindow& win, float px, float py,
                     const sf::Texture* texMiss) const {
  if (texMiss) {
    sf::Sprite s{*texMiss};
    auto sz = texMiss->getSize();
    float k = CELL_SIZE / static_cast<float>(std::max(sz.x, sz.y));
    s.setScale(k, k);
    s.setPosition(px, py);
    win.draw(s);
  } else {
    sf::CircleShape dot(4.f);
    dot.setFillColor(sf::Color::Black);
    dot.setPosition(px + CELL_SIZE / 2 - 4, py + CELL_SIZE / 2 - 4);
    win.draw(dot);
  }
}

void Board::drawCellSprites(sf::RenderWindow& win, int x, int y, int offX,
                            int offY, bool showShips,
                            const sf::Texture* texShip,
                            const sf::Texture* texHit,
                            const sf::Texture* texMiss) const {
  const float px = offX + x * CELL_SIZE;
  const float py = offY + y * CELL_SIZE;

  if (showShips && isShip(x, y))
    drawShip(win, px, py, texShip);

  if (!hitGrid[y][x])
    return;

  if (isShip(x, y))
    drawHit(win, px, py, texHit);
  else
    drawMiss(win, px, py, texMiss);
}