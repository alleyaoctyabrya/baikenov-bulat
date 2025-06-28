#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "Board.h"

class Player {
 public:
  Player();
  virtual ~Player() = default;

  Board& getBoard();

  void setName(const std::string& name);

  std::string getName() const;

  virtual void makeMove(Board& opponentBoard) { (void)opponentBoard; }

 protected:
  Board board;
  std::string name;
};

#endif
