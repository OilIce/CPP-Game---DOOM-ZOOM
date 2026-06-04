#pragma once

#include <constants.h>
#include <sstream>
class Database {
 public:
  struct Entry {
    std::string name;
    int stats[STAT_COUNT];
  };

  static Database& instance() {
    if (instance_ == nullptr) {
      instance_ = new Database();
    }

    return *instance_;
  }

  const Entry& get(int index) const {
    if (index >= 0 && index < entries_.size()) return entries_[index];
    return default_entry_;
  }

  int size() const { return entries_.size(); }

  void loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty()) continue;
      std::istringstream iss(line);
      Entry entry;

      if (!std::getline(iss, entry.name, '\t')) continue;
      for (int i = 0; i < STAT_COUNT; ++i) {
        std::string val;
        if (!std::getline(iss, val, '\t')) break;
        entry.stats[i] = std::stoi(val);
      }

      entries_.push_back(entry);
    }
  }

 private:
  Database() {
    loadFromFile(kAnimalInfo);
    default_entry_ = {"???", {10, 1, 0, 1}};
  }

  inline static Database* instance_ = nullptr;

  std::vector<Entry> entries_;
  Entry default_entry_;
};
