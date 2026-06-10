#pragma once

#include <map.h>

class Player {
 public:
  Player(const Map& map)
      : map_(map) { 
    auto xy_start = map.findStart();
    x_ = xy_start.first;
    y_ = xy_start.second;
  }

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

private:
  int x_, y_;
  const Map& map_;
  int flowers_ = 0;
};