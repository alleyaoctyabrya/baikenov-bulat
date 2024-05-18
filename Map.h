//
// Created by Bulat Baikenov on 01.05.2024.
//

#ifndef TP_PROJECT_MAP_H
#define TP_PROJECT_MAP_H
#include "Block.h"
#include <vector>
class Map {
private:
  std::vector<std::vector<Block>> blocks_;
  std::vector<std::vector<Block>> fake_blocks_;
  const size_t BLOCK_SIZE = 10;
  size_t dead_tiles_ = 0;
public:
  std::vector<int> ships_ = {4, 3, 2, 1};
  Map();

  Map(size_t size);

  void print();
  void wrote();
  Block& operator[](const size_t&);

  void change_to_ship(const size_t&, const size_t&);

  void change_to_blocked(const size_t&, const size_t&);

  void change_to_false(const size_t&, const size_t&);

  bool all_ships_zero() const;
  void place_random();

  void place_ship(const size_t&, const size_t&);

  bool can_place_ship(const size_t&, const size_t&);

  bool is_ships_ended(const size_t& length);

  bool is_lost() const;

  bool make_dead(const size_t&);
};


#endif //TP_PROJECT_MAP_H
