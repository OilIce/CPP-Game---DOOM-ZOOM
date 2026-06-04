#pragma once

#include <map.h>

class Player {
  int x_, y_;
  const IMap& map_;

 public:
  Player(const IMap& map, int start_x, int start_y)
      : x_(start_x), y_(start_y), map_(map) {}

  int x() const { return x_; }
  int y() const { return y_; }

  void setXY(int x, int y) { x_ = x; y_ = y; }

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