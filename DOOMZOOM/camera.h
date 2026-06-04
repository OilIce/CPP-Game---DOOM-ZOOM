#pragma once
#include <player.h>

class Camera {
  const IMap& map_;
  const Player& player_;

 public:
  Camera(const IMap& map, const Player& player) : map_(map), player_(player) {}

  std::pair<int, int> getOffset(int view_width, int view_height) const {
    int cam_x = player_.x() - view_width / 2;
    int cam_y = player_.y() - view_height / 2;
    cam_x = std::clamp(cam_x, 0, std::max(0, map_.width() - view_width));
    cam_y = std::clamp(cam_y, 0, std::max(0, map_.height() - view_height));
    return {cam_x, cam_y};
  }
};
