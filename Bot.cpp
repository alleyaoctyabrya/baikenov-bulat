//
// Created by Bulat Baikenov on 03.05.2024.
//
#include <random>
#include "Bot.h"

size_t Bot::attack() {
  return (rand() % 10 * 10 + rand() % 10);
}

bool Bot::lost() const {
  return map.is_lost();
}