#include "camera.h"

Camera::Camera(const Map& map, const Player& player)
    : map_(map), player_(player) {}

std::pair<int, int> Camera::getOffset(int view_width, int view_height) const {
  int cam_x = player_.x() - view_width / 2;
  int cam_y = player_.y() - view_height / 2;
  cam_x = std::clamp(cam_x, 0, std::max(0, map_.width() - view_width));
  cam_y = std::clamp(cam_y, 0, std::max(0, map_.height() - view_height));
  return {cam_x, cam_y};
}