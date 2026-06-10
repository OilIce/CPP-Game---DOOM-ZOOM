#pragma once
#include <animal.h>
#include <constants.h>

class Inventory {
 public:
  Inventory(std::vector<std::shared_ptr<Animal>> animals);

  inline const std::vector<std::shared_ptr<Animal>>& team() const { return team_; }
  inline std::vector<std::shared_ptr<Animal>>& team() { return team_; }
  inline const std::vector<std::string>& names() const { return names_; }
  inline std::vector<std::string>& names() { return names_; }
  inline int selected() const { return selected_; }
  inline int& selectedRef() { return selected_; }
  inline std::string getDescription() {
    return team_.empty() ? "" : team_[selected_].get()->shortInfo();
  }
  bool transferTo(Inventory& other, int min_limit = 0,
                  int max_limit = INT32_MAX);

 private:
  std::vector<std::shared_ptr<Animal>> team_;
  std::vector<std::string> names_;
  int selected_ = 0;
};
