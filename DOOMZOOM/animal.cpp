#include "animal.h"

Animal::Animal(int index) : index_(index), defending_(false) {
  const auto& entry = Database::instance().getEntry(index);
  name_ = entry.name;
  for (int i = 0; i < STAT_COUNT; ++i) stats_[i] = entry.stats[i];
  currentHp_ = stats_[HP];
  abilities_[0] = Database::instance().getAbility(2 * index);
  abilities_[1] = Database::instance().getAbility(2 * index + 1);
}

void Animal::takeDamage(int dmg) {
  int actual = std::max(0, dmg - def());
  currentHp_ = std::max(0, currentHp_ - actual);
  defending_ = false;
}

void Animal::modifyStats() {
  auto& database = Database::instance();
  for (auto& status : statuses_) {
    const Ability& ability = database.getAbility(status.ability_idx);
    for (size_t i = 0; i < ability.stat_indices.size(); ++i) {
      if (status.turns_left[i] != 0) {
        --status.turns_left[i];
        if (ability.stat_indices[i] == HP) {
          if (ability.magnitudes[i] > 0)
            heal(ability.magnitudes[i]);
          else if (ability.magnitudes[i] < 0)
            takeDamage(-ability.magnitudes[i]);
        } else {
          stats_[ability.stat_indices[i]] += ability.magnitudes[i];
        }
      }
    }
  }
}

std::string Animal::shortInfo() const {
  std::ostringstream oss;
  oss << name_ << " HP:" << currentHp_ << "/" << maxHp() << " ATK:" << atk()
      << " DEF:" << def() << " SPD:" << spd();
  return oss.str();
}

void Animal::updateStatuses() {
  auto it = statuses_.begin();
  while (it != statuses_.end()) {
    bool is_expired = std::all_of(it->turns_left.begin(), it->turns_left.end(),
                                  [](int i) { return i == 0; });
    if (is_expired) {
      const Ability& ability = Database::instance().getAbility(it->ability_idx);
      for (int i = 0; i < ability.stat_indices.size(); ++i) {
        if (ability.stat_indices[i] != HP) {
          stats_[ability.stat_indices[i]] -=
              ability.magnitudes[i] * ability.durations[i];
        }
      }

      it = statuses_.erase(it);
      continue;
    }

    ++it;
  }
}

void Animal::restoreStats() {
  const auto& entry = Database::instance().getEntry(index_);
  for (int i = 0; i < STAT_COUNT; ++i) stats_[i] = entry.stats[i];
}

void Animal::addStatus(const Ability& ability, int index) {
  for (auto& s : statuses_) {
    if (s.ability_idx == index) return;
  }

  ActiveStatus status;
  status.ability_idx = index;
  status.turns_left = ability.durations;
  statuses_.push_back(status);
}