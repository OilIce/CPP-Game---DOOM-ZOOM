#include "database.h"

Database::Database() {
  loadStats(kAnimalStats);
  loadAbilities(kAnimalAbilities, kDescriptions);
  default_entry_ = {"???", {10, 1, 0, 1}};
}

Database& Database::instance() {
  if (instance_ == nullptr) {
    instance_ = new Database();
  }

  return *instance_;
}

const Entry& Database::getEntry(int index) const {
  if (index >= 0 && index < entries_.size()) return entries_[index];
  return default_entry_;
}

const Ability& Database::getAbility(int index) const {
  if (index >= 0 && index < abilities_.size()) return abilities_[index];
  return abilities_[0];
}

void Database::loadStats(const std::string& filename) {
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

void Database::loadAbilities(const std::string& filename_abilities,
                             const std::string& filename_desc) {
  std::ifstream file_ab(filename_abilities);
  std::ifstream file_desc(filename_desc);
  if (!file_ab.is_open()) return;
  std::string line;

  while (std::getline(file_ab, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    Ability ability;

    if (!std::getline(iss, ability.name_, '\t')) continue;
    if (!std::getline(file_desc, ability.description)) continue;
    std::string type_str, stats_str, mags_str, durs_str;
    if (!std::getline(iss, type_str, '\t')) continue;
    if (!std::getline(iss, stats_str, '\t')) continue;
    if (!std::getline(iss, mags_str, '\t')) continue;
    if (!std::getline(iss, durs_str, '\t')) continue;
    ability.target_type_ = std::stoi(type_str);

    auto parse_int_list = [](const std::string& s, std::vector<int>& out) {
      std::istringstream ss(s);
      std::string number;
      while (std::getline(ss, number, ',')) {
        out.push_back(std::stoi(number));
      }
    };

    parse_int_list(stats_str, ability.stat_indices);
    parse_int_list(mags_str, ability.magnitudes);
    parse_int_list(durs_str, ability.durations);

    if (ability.stat_indices.size() == ability.magnitudes.size() &&
        ability.magnitudes.size() == ability.durations.size()) {
      abilities_.push_back(ability);
    }
  }
}