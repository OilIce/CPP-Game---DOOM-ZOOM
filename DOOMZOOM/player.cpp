#include "Player.h"

Player::Player(const Map& map) : map_(map) {
  auto xy_start = map.findStart();
  x_ = xy_start.first;
  y_ = xy_start.second;
}

bool Player::move(int dx, int dy) {
  int new_x = x_ + dx;
  int new_y = y_ + dy;
  if (map_.isWalkable(new_x, new_y)) {
    x_ = new_x;
    y_ = new_y;
    return true;
  }

  return false;
}