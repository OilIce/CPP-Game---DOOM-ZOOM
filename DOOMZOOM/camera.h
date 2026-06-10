#pragma once
#include <player.h>

class Camera {
 public:
  Camera(const Map& map, const Player& player);
  std::pair<int, int> getOffset(int view_width, int view_height) const;

 private:
  const Map& map_;
  const Player& player_;
};
