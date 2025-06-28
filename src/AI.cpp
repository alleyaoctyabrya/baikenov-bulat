#include "AI.h"

AI::AI() : Player() {
  resetAI();
  static bool seeded = false;
  if (!seeded) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    seeded = true;
  }
}

void AI::resetAI() {
  targetList.clear();
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      shotGrid[i][j] = false;
    }
  }
  lastShotResult = ShotResult::Miss;
}

sf::Vector2i AI::getRandomTarget() {
  int x, y;
  do {
    x = std::rand() % 10;
    y = std::rand() % 10;
  } while (shotGrid[y][x]);
  return sf::Vector2i(x, y);
}

void AI::addTargetNeighbors(int x, int y) {
  sf::Vector2i neighbors[4] = {sf::Vector2i(x, y - 1), sf::Vector2i(x, y + 1),
                               sf::Vector2i(x - 1, y), sf::Vector2i(x + 1, y)};
  for (auto n : neighbors) {
    int nx = n.x, ny = n.y;
    if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && !shotGrid[ny][nx]) {
      bool exists = false;
      for (const auto& t : targetList) {
        if (t.x == nx && t.y == ny) {
          exists = true;
          break;
        }
      }
      if (!exists) {
        targetList.push_back(sf::Vector2i(nx, ny));
      }
    }
  }
}

void AI::makeMove(Board& opponentBoard) {
  sf::Vector2i target;
  if (!targetList.empty()) {
    target = targetList.back();
    targetList.pop_back();
    if (shotGrid[target.y][target.x]) {
      return makeMove(opponentBoard);
    }
  } else {
    target = getRandomTarget();
  }
  int tx = target.x;
  int ty = target.y;
  shotGrid[ty][tx] = true;
  ShotResult result = opponentBoard.receiveShot(tx, ty);
  lastShotResult = result;
  if (result == ShotResult::Hit) {
    if (result != ShotResult::Sunk && result != ShotResult::AllSunk) {
      addTargetNeighbors(tx, ty);
    }
  } else if (result == ShotResult::Sunk) {
    targetList.clear();
  }

  if (result == ShotResult::AllSunk) {
    targetList.clear();
  }
}

ShotResult AI::getLastResult() const { return lastShotResult; }
