#pragma once
#include <constants.h>

class Map {
 public:
  Map(const std::string& filename) { loadFromFile(filename); }

  void loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error: cannot open " << filename << "\n";
      exit(EXIT_FAILURE);
    }

    data_.clear();

    std::string line;
    while (std::getline(file, line))  {
        data_.push_back(line);
    }

    if (data_.empty()) throw std::runtime_error("Map is empty\n");
  }

  bool isWalkable(int x, int y) const {
    if (y < 0 || y >= (data_.size())) return false;
    if (x < 0 || x >= (data_[y].size())) return false;
    return data_[y][x] == '0';
  }

  int width() const { return data_.empty() ? 0 : data_[0].size(); }
  int height() const { return data_.size(); }

  std::pair<int, int> findStart() const {
    for (int y = height() / 2; y < height(); ++y)
      for (int x = width() / 2; x < width(); ++x)
        if (data_[y][x] == '0') return {x, y};
    return {0, 0};
  }

  char at(int x, int y) const {
    if (!isWalkable(x, y) && x >= 0 && y >= 0 && x < width() && y < height())
      return (data_[y][x] == '1') ? '#' : '?';
    return '.';
  }

private:
  std::vector<std::string> data_;
};
