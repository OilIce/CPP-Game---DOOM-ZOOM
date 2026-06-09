#pragma once
#include "constants.h"

struct Ability {
  std::string description;
  std::string name_;
  int target_type_;
  std::vector<int> stat_indices;  
  std::vector<int> magnitudes;
  std::vector<int> durations;

  bool hasStat(int stat_idx) const {
    for (auto s : stat_indices)
      if (s == stat_idx) return true;
    return false;
  }
};
