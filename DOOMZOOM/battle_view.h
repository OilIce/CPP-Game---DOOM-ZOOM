#pragma once
#include <animal.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <random>

using namespace ftxui;
using Team = std::vector<Animal>;

class BattleView {
 public:
  BattleView(Team& player, Team& enemy, int flowers);
  inline bool isOver() const { return current_phase_ == Phase::BattleOver; }
  inline bool isVictory() const { return !std::any_of(enemy_team_.begin(), enemy_team_.end(), [](auto& a) { return a.isAlive(); });}
  inline int getFlowersleft() const { return flowers_; }
  inline Component getComponent() { return main_component_; }

 private:
  Team player_team_;
  Team enemy_team_;
  std::vector<Animal*> all_fighters_;
  std::vector<bool> is_player_team_;
  std::vector<int> turn_order_;
  int flowers_;

  Phase current_phase_ = Phase::ChooseAction;
  std::string message_;

  Component action_menu_;
  Component target_menu_;
  Component main_component_;

  std::vector<std::string> target_entries_;
  int action_menu_index_;
  int target_menu_index_;
  int selected_action_;
  int selected_target_;
  int current_turn_idx_ = 0;

  void buildTargetMenu(Team* target_team);
  void buildTurnOrder();
  void buildUI();
  void checkVictory();
  void executeAction(int action, int target_idx);
  void enemyTurn();
  void nextTurn();
  void removeDead(Team& team);
};
