#pragma once

#include <map.h>

class Player {
 public:
  Player(const Map& map);
  inline void addFlower() { ++flowers_; }
  inline void setFlowers(int leftovers) { flowers_ = leftovers; }
  inline int getFlowers() const { return flowers_; }
  bool move(int dx, int dy);
  inline void setXY(int x, int y) {
    x_ = x;
    y_ = y;
  }

  inline int x() const { return x_; }
  inline int y() const { return y_; }
  

private:
  int x_, y_;
  const Map& map_;
  int flowers_ = 0;
};