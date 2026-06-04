#pragma once
#include <animal.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <random>
#include <numeric>

using namespace ftxui;
using Team = std::vector<std::shared_ptr<Animal>>;

class BattleView {
 public:
  BattleView(Team player, Team enemy, ScreenInteractive& screen)
      : player_team_(std::move(player)),
        enemy_team_(std::move(enemy)),
        screen_(screen),
        rng_(std::random_device{}()) {
    buildTurnOrder();
    buildUI();
    message_ = "Battle begins!";
  }

  bool isOver() const { return current_phase_ == Phase::BattleOver; }
  bool isVictory() const { return enemy_team_.empty(); }

  Component getComponent() { return main_component_; }

 private:
  Team player_team_;
  Team enemy_team_;
  ScreenInteractive& screen_;
  std::mt19937 rng_;

  Phase current_phase_ = Phase::ChooseAction;
  std::string message_;

  Team all_fighters_;
  std::vector<Team*> _team_of_fighter_;
  std::vector<int> turn_order_;
  int current_turn_idx_ = 0;

  int selected_action_ = 0;
  int selected_target_ = 0;

  Component action_menu_;
  Component target_menu_;
  Component main_component_;

  std::vector<std::string> target_entries_;
  int action_menu_index_ = 0;
  int target_menu_index_ = 0;


  void buildTurnOrder() {
    all_fighters_.clear();
    _team_of_fighter_.clear();
    for (auto& a : player_team_) {
      all_fighters_.push_back(a);
      _team_of_fighter_.push_back(&player_team_);
    }
    for (auto& a : enemy_team_) {
      all_fighters_.push_back(a);
      _team_of_fighter_.push_back(&enemy_team_);
    }

    std::vector<int> indices(all_fighters_.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
      return all_fighters_[a]->spd() > all_fighters_[b]->spd();
    });

    turn_order_ = indices;
    current_turn_idx_ = 0;
    while (current_turn_idx_ < (int)turn_order_.size() &&
           !all_fighters_[turn_order_[current_turn_idx_]]->isAlive()) 
        current_turn_idx_++;
  }

  void nextTurn() {
    for (int i = 0; i < (int)turn_order_.size(); ++i) {
        current_turn_idx_ = (current_turn_idx_ + 1) % turn_order_.size();
      int idx = turn_order_[current_turn_idx_];
      if (all_fighters_[idx]->isAlive()) break;
    }
    current_phase_ = Phase::ChooseAction;
    checkVictory();
  }

  void checkVictory() {
    bool player_alive = std::any_of(player_team_.begin(), player_team_.end(),
                                   [](auto& a) { return a->isAlive(); });
    bool enemy_alive = std::any_of(enemy_team_.begin(), enemy_team_.end(),
                                  [](auto& a) { return a->isAlive(); });
    if (!player_alive) {
      current_phase_ = Phase::BattleOver;
      message_ = "Your team has been defeated!";
    } else if (!enemy_alive) {
      current_phase_ = Phase::BattleOver;
      message_ = "You won the battle!";
    }
  }

  void executeAction(int action, int target_idx) {
    int fighterIdx = turn_order_[current_turn_idx_];
    auto& fighter = all_fighters_[fighterIdx];
    Team* my_team = _team_of_fighter_[fighterIdx];
    Team* opp_team = (my_team == &player_team_) ? &enemy_team_ : &player_team_;

    switch (action) {
      case Action::Attack: {
        if (opp_team->empty()) return;
        auto& target = (*opp_team)[target_idx];
        int dmg = fighter->atk();
        target->takeDamage(dmg);
        message_ = fighter->name() + " attacks " + target->name() + " for " +
                   std::to_string(dmg) + " damage.";
        break;
      }
      case Action::Heal: {
        if (my_team->empty()) return;
        auto& target = (*my_team)[target_idx];
        int heal_amt = 10;
        target->heal(heal_amt);
        message_ = fighter->name() + " heals " + target->name() + " by " +
                   std::to_string(heal_amt) + ".";
        break;
      }
      case Action::Defend:
        fighter->setDefending(true);
        message_ = fighter->name() + " defends.";
        break;
    }

    for (auto& a : all_fighters_)
      if (a != fighter) a->setDefending(false);

    player_team_.erase(std::remove_if(player_team_.begin(), player_team_.end(),
                                     [](auto& a) { return !a->isAlive(); }),
                      player_team_.end());
    enemy_team_.erase(std::remove_if(enemy_team_.begin(), enemy_team_.end(),
                                    [](auto& a) { return !a->isAlive(); }),
                     enemy_team_.end());

    buildTurnOrder();
    auto it = std::find(turn_order_.begin(), turn_order_.end(), fighterIdx);
    if (it != turn_order_.end())
      current_turn_idx_ = std::distance(turn_order_.begin(), it);
    nextTurn();
  }

  void enemyTurn() {
    int fighter_idx = turn_order_[current_turn_idx_];
    Team* my_team = _team_of_fighter_[fighter_idx];
    Team* opp_team = (my_team == &player_team_) ? &enemy_team_ : &player_team_;

    std::uniform_int_distribution<int> act_dist(0, 2);
    int action = act_dist(rng_);
    if (action == Action::Attack && opp_team->empty()) action = Action::Defend;
    if (action == Action::Heal && my_team->empty()) action = Action::Defend;

    int target = 0;
    if (action == Action::Attack && !opp_team->empty())
      target = std::uniform_int_distribution<int>(0, opp_team->size() - 1)(rng_);
    else if (action == Action::Heal && !my_team->empty())
      target = std::uniform_int_distribution<int>(0, my_team->size() - 1)(rng_);

    executeAction(action, target);
  }

  void buildUI() {
    action_menu_index_ = 0;
    MenuOption action_opt;
    action_opt.on_enter = [this]() {
      if (current_phase_ != Phase::ChooseAction) return;
      int fighter_idx = turn_order_[current_turn_idx_];
      Team* my_team =
          _team_of_fighter_[fighter_idx];
      Team* opp_team =
          (my_team == &player_team_) ? &enemy_team_ : &player_team_;

      if (action_menu_index_ == 0) {
        if (opp_team->empty()) return;
        selected_action_ = 0;
        buildTargetMenu(opp_team);
        current_phase_ = Phase::ChooseTarget;
      } else if (action_menu_index_ == 1) {
        if (my_team->empty()) return;
        selected_action_ = 1;
        buildTargetMenu(my_team);
        current_phase_ = Phase::ChooseTarget;
      } else {
        executeAction(2, 0);
      }
    };

    action_menu_ = Menu(&kActionEntries_, &action_menu_index_, action_opt);

    target_menu_ = Renderer([] { return text(""); });

    auto main_renderer = Renderer([this] {
      Elements p_lines, e_lines;
      p_lines.push_back(text(" Your team ") | bold);
      for (auto& a : player_team_) p_lines.push_back(text("  " + a->shortInfo()));
      if (player_team_.empty()) p_lines.push_back(text("  (empty)"));
      e_lines.push_back(text(" Enemy team ") | bold);
      for (auto& a : enemy_team_) e_lines.push_back(text("  " + a->shortInfo()));
      if (enemy_team_.empty()) e_lines.push_back(text("  (empty)"));

      auto teams = hbox(
          {vbox(std::move(p_lines)) | border, vbox(std::move(e_lines)) | border});

      auto msg = text(message_) | center;

      Element action_panel;
      if (current_phase_ == Phase::BattleOver) {
        action_panel = text("Press any key to exit.") | center | bold;
      } else if (current_phase_ == Phase::ChooseAction) {
        int idx = turn_order_[current_turn_idx_];
        bool is_players_choice = std::find(player_team_.begin(), player_team_.end(), all_fighters_[idx]) != player_team_.end();
        std::string turn_name = all_fighters_[idx]->name();
        std::string choice_text = (is_players_choice) ? " (your choice)" : " (enemy's choice)";
        action_panel = vbox({text("Turn: " + turn_name + choice_text) | bold,
                            action_menu_->Render()});
      } else if (current_phase_ == Phase::ChooseTarget) {
        action_panel =
            vbox({text("Choose target:") | bold, target_menu_->Render()});
      }

      return vbox({teams, separator(), msg, separator(), action_panel}) | border;
    });

    main_component_ = main_renderer | CatchEvent([this](Event event) {
                       if (current_phase_ == Phase::BattleOver) {
                         screen_.ExitLoopClosure()();
                         return true;
                       }

                       int fighter_idx = turn_order_[current_turn_idx_];
                       if (_team_of_fighter_[fighter_idx] == &enemy_team_) {
                         enemyTurn();
                         return true;
                       }

                       if (current_phase_ == Phase::ChooseAction)
                         return action_menu_->OnEvent(event);
                       else if (current_phase_ == Phase::ChooseTarget)
                         return target_menu_->OnEvent(event);
                       return false;
                     });
  }

  void buildTargetMenu(Team* target_team) {
    target_entries_.clear();
    for (auto& a : *target_team) target_entries_.push_back(a->shortInfo());
    target_menu_index_ = 0;
    MenuOption opt;
    opt.on_enter = [this]() {
      executeAction(selected_action_, target_menu_index_);
    };

    target_menu_ = Menu(&target_entries_, &target_menu_index_, opt);
  }
};
