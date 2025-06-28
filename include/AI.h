#ifndef AI_H
#define AI_H

#include <SFML/System/Vector2.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "Player.h"

class AI : public Player {
 public:
  AI();

  void makeMove(Board& opponentBoard) override;

  void resetAI();

  ShotResult getLastResult() const;

 private:
  bool shotGrid[10][10];

  std::vector<sf::Vector2i> targetList;

  ShotResult lastShotResult;

  sf::Vector2i getRandomTarget();

  void addTargetNeighbors(int x, int y);
};

#endif
