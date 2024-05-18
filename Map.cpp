//
// Created by Bulat Baikenov on 01.05.2024.
//

#include "Map.h"
#include <ncurses.h>
#include <fstream>
#include <string>
Map::Map() {
  blocks_.resize(BLOCK_SIZE);
  for (auto& it : blocks_) {
    it.resize(BLOCK_SIZE);
  }
  wrote();
}
void Map::wrote() {
  std::ofstream myfile("~/cfg");
  for (size_t i = 0; i < BLOCK_SIZE; ++i) {
    for (size_t j = 0; j < BLOCK_SIZE; ++j) {
      myfile << blocks_[i][j].handle() << " ";
    }
    myfile << "\n";
  }
  myfile << std::endl;
}
void Map::print() {
  printw("                        remaining ships to place:\n");
  printw("                        one-tile ships : %d\n", ships_[0]);
  printw("                        two-tile ships : %d\n", ships_[1]);
  printw("                        three-tile ships : %d\n", ships_[2]);
  printw("                        four-tile ships : %d\n", ships_[3]);



  printw("  ");
  for (size_t i = 0; i < BLOCK_SIZE; ++i) {
    printw((std::to_string(i) + " ").c_str());
  }
  printw("\n");
  for (size_t j = 0; j < BLOCK_SIZE; ++j) {
    addch('A' + j);
    printw(" ");
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
      printw((blocks_[j][i].handle() + " ").c_str());
    }
    printw("\n");
  }
}
Block& Map::operator[](const size_t& block) {
  return blocks_[block / 10][block % 10];
}
void Map::change_to_ship(const size_t& first, const size_t& second) {
  blocks_[first][second].set_ship();
  wrote();
}

void Map::change_to_blocked(const size_t& first, const size_t& second) {
  blocks_[first][second].set_block();
  wrote();
}

void Map::change_to_false(const size_t& first, const size_t& second) {
  blocks_[first][second].set_false();
  wrote();
}

bool Map::all_ships_zero() const {
  return ships_[0] == 0 && ships_[1] == 0 && ships_[2] == 0 && ships_[3] == 0;
}

void Map::place_random() {
  while (true) {
    char first_char = rand() % 10 + 41, second_char;
    int first_int = rand() % 10, second_int;
    if (rand() % 2 == 1) {
      second_char = first_char;
      second_int = (rand() % 4 + first_int) % 10;
    } else {
      second_int = first_int;
      second_char = (first_int % 41 + rand() % 4) % 10 + 41;
    }
    size_t length = std::abs(first_int - second_int) + std::abs(second_char - first_char);
    if (length <= 4 && is_ships_ended(length)) {
      if (!can_place_ship((first_char - 'A') * 10 + first_int, (second_char - 'A') * 10 + second_int)) {
        continue;
      } else {
        place_ship((first_char - 'A') * 10 + first_int,(second_char - 'A') * 10 + second_int);
        break;
      }
    } else {
      continue;
    }
  }
  wrote();
}
bool Map::is_ships_ended(const size_t &length) {
  if ((ships_[3] != 0 && length < 4) || (ships_[2] != 0 && length < 3)
  || (ships_[1] != 0 && length < 2)) {
    return false;
  }
  return true;
}
bool Map::can_place_ship(const size_t& first_point, const size_t& second_point) {
  size_t diff = 1;
  if (second_point - first_point >= 4) {
    diff = 10;
  }
  for (size_t i = first_point; i <= second_point; i += diff) {
    if (Map::operator[](i).is_false() || Map::operator[](i).is_ship()) {
      return false;
    }
  }
  return true;
}

void Map::place_ship(const size_t& first_point, const size_t& second_point) {
  size_t diff = 1;
  if (second_point - first_point >= 4) {
    diff = 10;
  }
  for (size_t i = first_point; i <= second_point; i += diff) {
    Map::operator[](i).set_ship();
  }
  wrote();
}

bool Map::is_lost() const {
  return dead_tiles_ == 20;
}

bool Map::make_dead(const size_t& index) {
  if (!Map::operator[](index).is_blocked()) {
    Map::operator[](index).set_block();
    if (Map::operator[](index).is_ship()) {
      dead_tiles_++;
    }
    return true;
  } else {
    return false;
  }
  wrote();
}
