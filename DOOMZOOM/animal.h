#pragma once
#include <database.h>

struct ActiveStatus {
  int ability_idx;
  std::vector<int> turns_left;
};

class Animal {
 public:
  Animal(int index) : index_(index), defending_(false) {
    const auto& entry = Database::instance().getEntry(index);
    name_ = entry.name;
    for (int i = 0; i < STAT_COUNT; ++i) stats_[i] = entry.stats[i];
    currentHp_ = stats_[HP];
    abilities_[0] = Database::instance().getAbility(2 * index);
    abilities_[1] = Database::instance().getAbility(2 * index + 1);
  }

  const std::string& name() const { return name_; }
  const Ability& getAbility(bool idx) const { return abilities_[idx]; }
  const int getIndex() const { return index_; }
  int hp() const { return currentHp_; }
  int maxHp() const { return stats_[HP]; }
  int atk() const { return stats_[ATK]; }
  int def() const { return stats_[DEF] + (defending_ ? kDefendingEffect : 0); }
  int spd() const { return stats_[SPD]; }
  bool isAlive() const { return currentHp_ > 0; }

  void takeDamage(int dmg) {
    int actual = std::max(0, dmg - def());
    currentHp_ = std::max(0, currentHp_ - actual);
    defending_ = false;
  }

  void heal() { currentHp_ = maxHp(); }
  void heal(int amount) { currentHp_ = std::min(maxHp(), currentHp_ + amount); }
  void modifyStats() {
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

  void setDefending(bool v) { defending_ = v; }
  std::string shortInfo() const {
    std::ostringstream oss;
    oss << name_ << " HP:" << currentHp_ << "/" << maxHp() << " ATK:" << atk()
        << " DEF:" << def() << " SPD:" << spd();
    return oss.str();
  }

  void clearStatuses() { statuses_.clear(); }
  void updateStatuses() {
    auto it = statuses_.begin();
    while (it != statuses_.end()) {
      bool is_expired =
          std::all_of(it->turns_left.begin(), it->turns_left.end(),
                      [](int i) { return i == 0; });
      if (is_expired) {
        const Ability& ability =
            Database::instance().getAbility(it->ability_idx);
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

  void restoreStats() {
    const auto& entry = Database::instance().getEntry(index_);
    for (int i = 0; i < STAT_COUNT; ++i) stats_[i] = entry.stats[i];
  }

  void addStatus(const Ability& ability, int index) {
    for (auto& s : statuses_) {
      if (s.ability_idx == index) return;
    }

    ActiveStatus status;
    status.ability_idx = index;
    status.turns_left = ability.durations;
    statuses_.push_back(status);
  }

 private:
  std::string name_;
  int stats_[STAT_COUNT];
  int index_;
  int currentHp_;
  bool defending_;
  Ability abilities_[2];
  bool ability_active_[2];

  std::vector<ActiveStatus> statuses_;
};