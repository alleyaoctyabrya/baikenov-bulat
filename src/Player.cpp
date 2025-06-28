#include "Player.h"

Player::Player() {
  board.reset(true);
  name = "";
}

Board& Player::getBoard() { return board; }

void Player::setName(const std::string& name) { this->name = name; }

std::string Player::getName() const { return name; }
