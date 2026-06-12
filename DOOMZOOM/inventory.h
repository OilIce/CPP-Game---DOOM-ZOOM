#pragma once
#include <animal.h>

using Team = std::vector<Animal>;

class Inventory {
 public:
  Inventory(Team animals);

  inline const Team& team() const { return team_; }
  inline Team& team() { return team_; }
  inline const std::vector<std::string>& names() const { return names_; }
  inline std::vector<std::string>& names() { return names_; }
  inline int selected() const { return selected_; }
  inline int& selectedRef() { return selected_; }
  inline std::string getDescription() {
    return team_.empty() ? "" : team_[selected_].shortInfo();
  }
  bool transferTo(Inventory& other, int min_limit = 0,
                  int max_limit = INT32_MAX);

 private:
  Team team_;
  std::vector<std::string> names_;
  int selected_ = 0;
};
