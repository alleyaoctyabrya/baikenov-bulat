#include "Block.h"

Block::Block() : is_blocked_(false), is_ship_(false) , is_false_(false) {}

bool Block::is_ship() const noexcept {
  return is_ship_;
}

bool Block::is_blocked() const noexcept {
  return is_blocked_;
}

std::string Block::handle() const noexcept {
  if (is_false()) {
    return ":";
  } else if (is_ship()) {
    if (is_blocked()) {
      return "x";
    } else {
      return "<";
    }
  } else {
    if (is_blocked()) {
      return "%";
    } else {
      return "#";
    }
  }
}

void Block::set_block() noexcept {
  is_blocked_ = !is_blocked_;
}
void Block::set_ship() noexcept {
  is_ship_ = !is_ship_;
}
bool Block::is_false() const noexcept {
  return is_false_;
}
void Block::set_false() noexcept {
  is_false_ = !is_false_;
}
std::ostream& operator<<(std::ostream& stream, const Block& block) noexcept {
  stream << block.handle();
  return stream;
}