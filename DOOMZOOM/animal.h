#pragma once
#include <database.h>

struct ActiveStatus {
  int ability_idx;
  std::vector<int> turns_left;
};

class Animal {
 public:
  Animal(int index);

  inline const std::string& name() const { return name_; }
  inline const Ability& getAbility(bool idx) const { return abilities_[idx]; }
  inline const int getIndex() const { return index_; }
  inline int hp() const { return currentHp_; }
  inline int maxHp() const { return stats_[HP]; }
  inline int atk() const { return stats_[ATK]; }
  inline int def() const { return stats_[DEF] + (defending_ ? kDefendingEffect : 0); }
  inline int spd() const { return stats_[SPD]; }
  inline bool isAlive() const { return currentHp_ > 0; }
  inline void heal() { currentHp_ = maxHp(); }
  inline void heal(int amount) { currentHp_ = std::min(maxHp(), currentHp_ + amount); }
  inline void setDefending(bool v) { defending_ = v; }
  inline void clearStatuses() { statuses_.clear(); }

  std::string shortInfo() const;
  void modifyStats();
  void takeDamage(int dmg);
  void updateStatuses();
  void restoreStats();
  void addStatus(const Ability& ability, int index);

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