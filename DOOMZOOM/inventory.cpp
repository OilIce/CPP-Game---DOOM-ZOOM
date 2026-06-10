#include "Inventory.h"

Inventory::Inventory(std::vector<std::shared_ptr<Animal>> animals)
    : team_(std::move(animals)) {
  for (auto animal : team_) {
    names_.push_back(animal.get()->name());
  }
}

bool Inventory::transferTo(Inventory& other, int min_limit,
                           int max_limit) {
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