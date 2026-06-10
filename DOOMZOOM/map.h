#pragma once
#include <constants.h>

class Map {
 public:
  Map(const std::string& filename);
  char at(int x, int y) const;
  std::pair<int, int> findStart() const;
  void loadFromFile(const std::string& filename);
  bool isWalkable(int x, int y) const;

  inline int height() const { return data_.size(); }
  inline int width() const { return data_.empty() ? 0 : data_[0].size(); }
private:
  std::vector<std::string> data_;
};
