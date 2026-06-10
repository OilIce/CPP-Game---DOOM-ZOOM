#pragma once

#include <constants.h>

#include <sstream>

struct Ability {
  std::string description;
  std::string name_;
  int target_type_;
  std::vector<int> stat_indices;
  std::vector<int> magnitudes;
  std::vector<int> durations;
};

struct Entry {
  std::string name;
  int stats[STAT_COUNT];
};

class Database {
 public:
  static Database& instance();

  const Entry& getEntry(int index) const;
  const Ability& getAbility(int index) const;
  void loadStats(const std::string& filename);
  void loadAbilities(const std::string& filename_abilities,
                     const std::string& filename_desc);
  inline int size() const { return entries_.size(); }

 private:
  Database();

  inline static Database* instance_ = nullptr;
  std::vector<Ability> abilities_;
  std::vector<Entry> entries_;
  Entry default_entry_;
};
