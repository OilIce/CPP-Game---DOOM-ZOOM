#pragma once
#include <database.h>

class Animal {
 public:
  Animal(int index) : index_(index), defending_(false) {
    const auto& entry = Database::instance().get(index);
    name_ = entry.name;
    for (int i = 0; i < STAT_COUNT; ++i) params_[i] = entry.stats[i];
    currentHp_ = params_[HP];
  }

  const std::string& name() const { return name_; }
  int hp() const { return currentHp_; }
  int maxHp() const { return params_[HP]; }
  int atk() const { return params_[ATK]; }
  int def() const { return params_[DEF] + (defending_ ? 5 : 0); }
  int spd() const { return params_[SPD]; }
  bool isAlive() const { return currentHp_ > 0; }

  void takeDamage(int dmg) {
    int actual = std::max(0, dmg - def());
    currentHp_ = std::max(0, currentHp_ - actual);
    defending_ = false;
  }

  void heal() { currentHp_ = maxHp(); }
  void heal(int amount) { currentHp_ = std::min(maxHp(), currentHp_ + amount); }

  void setDefending(bool v) { defending_ = v; }
  std::string shortInfo() const {
    std::ostringstream oss;
    oss << name_ << " HP:" << currentHp_ << "/" << maxHp() << " ATK:" << atk()
        << " DEF:" << def() << " SPD:" << spd();
    return oss.str();
  }

 private:
  std::string name_;
  int params_[STAT_COUNT];
  int index_;
  int currentHp_;
  bool defending_;
};