#pragma once

#include <map.h>

class Player {
  int x_, y_;
  const Map& map_;
  int flowers_ = 0;

 public:
  Player(const Map& map, int start_x, int start_y)
      : x_(start_x), y_(start_y), map_(map) {}

  int x() const { return x_; }
  int y() const { return y_; }

  void setXY(int x, int y) { x_ = x; y_ = y; }
  void addFlower() { ++flowers_; }
  void setFlowers(int leftovers) { flowers_ = leftovers; }
  int getFlowers() const { return flowers_; }

  bool move(int dx, int dy) {
    int new_x = x_ + dx;
    int new_y = y_ + dy;
    if (map_.isWalkable(new_x, new_y)) {
      x_ = new_x;
      y_ = new_y;
      return true;
    }

    return false;
  }
};