#pragma once
#include <animal.h>
#include <constants.h>

class Inventory {
 public:
  Inventory(std::vector<std::shared_ptr<Animal>> animals)
      : team_(std::move(animals)) {
    for (auto animal : team_) {
      names_.push_back(animal.get()->name());
    }
  }

  const std::vector<std::shared_ptr<Animal>>& team() const { return team_; }
  std::vector<std::shared_ptr<Animal>>& team() { return team_; }
  const std::vector<std::string>& names() const { return names_; }
  std::vector<std::string>& names() { return names_; }
  int selected() const { return selected_; }
  int& selectedRef() { return selected_; }
  std::string getDescription() {
    return team_.empty() ? "" : team_[selected_].get()->shortInfo();
  }

  bool transferTo(Inventory& other, int min_limit = 0,
                  int max_limit = INT32_MAX) {
    if (team_.size() <= min_limit || other.team_.size() >= max_limit)
      return false;
    if (team_.empty()) return false;
    auto item = team_[selected_];
    team_.erase(team_.begin() + selected_);
    names_.erase(names_.begin() + selected_);
    other.team_.push_back(item);
    other.names_.push_back(item.get()->name());
    if (selected_ >= team_.size())
      selected_ = std::max(0, static_cast<int>(team_.size()) - 1);
    other.selected_ = other.team_.size() - 1;
    return true;
  }

 private:
  std::vector<std::shared_ptr<Animal>> team_;
  std::vector<std::string> names_;
  int selected_ = 0;
};
