#pragma once
#include <animal.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <numeric>
#include <random>

using namespace ftxui;
using Team = std::vector<std::shared_ptr<Animal>>;

class BattleView {
 public:
  BattleView(Team player, Team enemy, int flowers)
      : player_team_(player), enemy_team_(enemy), flowers_(flowers) {
    buildTurnOrder();
    buildUI();
    message_ = "Battle begins!";
  }

  bool isOver() const { return current_phase_ == Phase::BattleOver; }
  bool isVictory() const { 
    bool enemy_alive = std::any_of(enemy_team_.begin(), enemy_team_.end(), [](auto& a) { return a->isAlive(); });
    return !enemy_alive; 
  }

  int getFlowersleft() const { return flowers_; }

  Component getComponent() { return main_component_; }

 private:
  Team player_team_;
  Team enemy_team_;
  Team all_fighters_;
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

  void buildTurnOrder() {
    all_fighters_.clear();
    is_player_team_.clear();
    for (auto& a : player_team_) {
      all_fighters_.push_back(a);
      is_player_team_.push_back(true);
    }
    for (auto& a : enemy_team_) {
      all_fighters_.push_back(a);
      is_player_team_.push_back(false);
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

  void removeDead(Team& team) {
    team.erase(std::remove_if(team.begin(), team.end(), [](auto& a) { return !a->isAlive(); }), team.end());
  }

  void nextTurn() {
    int idx = turn_order_[current_turn_idx_];
    
    for (int i = 0; i < turn_order_.size(); ++i) {
      current_turn_idx_ = (current_turn_idx_ + 1) % turn_order_.size();
      idx = turn_order_[current_turn_idx_];
      if (all_fighters_[idx]->isAlive()) {
        all_fighters_[idx]->modifyStats();
        all_fighters_[idx]->updateStatuses();
      }

      if (all_fighters_[idx]->isAlive()) break;
    }

    removeDead(player_team_);
    removeDead(enemy_team_);
    current_phase_ = Phase::ChooseAction;
    checkVictory();
  }

  void checkVictory() {
    bool player_alive = std::any_of(player_team_.begin(), player_team_.end(),
                                    [](auto& a) { return a->isAlive(); });
    bool enemy_alive = std::any_of(enemy_team_.begin(), enemy_team_.end(),
                                   [](auto& a) { return a->isAlive(); });
    if (!enemy_alive) {
      current_phase_ = Phase::BattleOver;
      message_ = "You won the battle!";
    } else if (!player_alive) {
      current_phase_ = Phase::BattleOver;
      message_ = "Your team has been defeated!";
    }
  }

  void executeAction(int action, int target_idx) {
    int fighter_idx = turn_order_[current_turn_idx_];
    auto& fighter = all_fighters_[fighter_idx];
    Team* my_team =
        (is_player_team_[fighter_idx]) ? &player_team_ : &enemy_team_;
    Team* opp_team =
        (is_player_team_[fighter_idx]) ? &enemy_team_ : &player_team_;

    switch (action) {
      case Action::Attack: {
        if (opp_team->empty()) break;
        auto& target = (*opp_team)[target_idx];
        int dmg = fighter->atk();
        target->takeDamage(dmg);
        message_ = fighter->name() + " attacks " + target->name() + " for " +
                   std::to_string(dmg) + " damage.";
        break;
      }
      case Action::Heal: {
        if (my_team->empty() || !flowers_) break;
        --flowers_;
        auto& target = (*my_team)[target_idx];
        int heal_amt = kHealingEffect;
        target->heal(kHealingEffect);
        message_ = fighter->name() + " heals " + target->name() + " by " +
                   std::to_string(heal_amt) + ".";
        break;
      }
      case Action::Defend:
        fighter->setDefending(true);
        message_ = fighter->name() + " defends.";
        break;
      default: {
        auto& ability = fighter->getAbility(action - Action::Ability1);
        int ability_idx = 2 * fighter->getIndex() + action - Action::Ability1;
        switch (ability.target_type_) {
          case AbilityTarget::Enemy:
            (*opp_team)[target_idx]->addStatus(ability, ability_idx);
            message_ = fighter->name() + " used ability " + ability.name_ +
                       " on " + (*opp_team)[target_idx]->name();
            break;
          case AbilityTarget::Self:
            fighter->addStatus(ability, ability_idx);
            message_ = fighter->name() + " used ability " + ability.name_ +
                       " on itself";
            break;
          case AbilityTarget::Comrade:
            (*my_team)[target_idx]->addStatus(ability, ability_idx);
            message_ = fighter->name() + " used ability " + ability.name_ +
                       " on " + (*my_team)[target_idx]->name();
            break;
          case AbilityTarget::AllEnemies:
            for (auto& animal : *opp_team)
              animal->addStatus(ability, ability_idx);
            message_ = fighter->name() + " used ability " + ability.name_ +
                       " on everyone";
            break;
        }

        break;
      }
    }

    for (auto& a : all_fighters_)
      if (a != fighter) a->setDefending(false);

    removeDead(player_team_);
    removeDead(enemy_team_);

    buildTurnOrder();
    auto it = std::find(turn_order_.begin(), turn_order_.end(), fighter_idx);
    if (it != turn_order_.end())
      current_turn_idx_ = std::distance(turn_order_.begin(), it);
    nextTurn();
  }

  void enemyTurn() {
    int fighter = turn_order_[current_turn_idx_];
    Team* my_team = (is_player_team_[fighter]) ? &player_team_ : &enemy_team_;
    Team* opp_team = (is_player_team_[fighter]) ? &enemy_team_ : &player_team_;

    std::uniform_int_distribution<int> act_dist(Action::Attack,
                                                Action::Ability2);
    std::mt19937 engine(std::random_device{}());
    int action = act_dist(engine);
    if (action == Action::Attack && opp_team->empty()) action = Action::Defend;
    if (action == Action::Heal) action = Action::Defend;

    int target = 0;
    int ability_target = all_fighters_[fighter]->getAbility(action - Action::Ability1).target_type_;
    if (action == Action::Attack && !opp_team->empty() ||
        ((action == Action::Ability1 || action == Action::Ability2) &&
         ability_target == AbilityTarget::Enemy))
      target = std::uniform_int_distribution<int>(0, opp_team->size() - 1)(engine);
    else if (((action == Action::Ability1 || action == Action::Ability2) &&
              ability_target == AbilityTarget::Comrade))
      target = std::uniform_int_distribution<int>(0, my_team->size() - 1)(engine);

    executeAction(action, target);
  }

  void buildUI() {
    action_menu_index_ = Action::Attack;
    MenuOption action_opt;
    action_opt.on_enter = [this]() {
      if (current_phase_ != Phase::ChooseAction) return;
      int fighter = turn_order_[current_turn_idx_];
      Team* my_team = (is_player_team_[fighter]) ? &player_team_ : &enemy_team_;
      Team* opp_team =
          (is_player_team_[fighter]) ? &enemy_team_ : &player_team_;

      selected_action_ = action_menu_index_;
      switch (action_menu_index_) {
        case (Action::Attack): {
          if (opp_team->empty()) return;
          buildTargetMenu(opp_team);
          current_phase_ = Phase::ChooseTarget;
          break;
        }
        case (Action::Heal): {
          if (my_team->empty()) return;
          buildTargetMenu(my_team);
          current_phase_ = Phase::ChooseTarget;
          break;
        }
        case (Action::Defend): {
          executeAction(Action::Defend, 0);
          break;
        }
        default: {
          int target_type =
              all_fighters_[fighter]->getAbility(action_menu_index_ - Action::Ability1).target_type_;
          switch (target_type) {
            case AbilityTarget::Enemy: {
              buildTargetMenu(opp_team);
              current_phase_ = Phase::ChooseTarget;
              break;
            }
            case AbilityTarget::Comrade: {
              buildTargetMenu(my_team);
              current_phase_ = Phase::ChooseTarget;
              break;
            }
            default:
              executeAction(action_menu_index_, 0);
              break;
          }
        }
      }
    };

    action_menu_ = Menu(&kActionEntries_, &action_menu_index_, action_opt);

    target_menu_ = Renderer([] { return text(""); });

    auto main_renderer = Renderer([this] {
      Elements p_lines, e_lines;
      p_lines.push_back(text(" Your team ") | bold);
      for (auto& a : player_team_)
        p_lines.push_back(text("  " + a->shortInfo()));
      if (player_team_.empty()) p_lines.push_back(text("  (empty)"));
      e_lines.push_back(text(" Enemy team ") | bold);
      for (auto& a : enemy_team_)
        e_lines.push_back(text("  " + a->shortInfo()));
      if (enemy_team_.empty()) e_lines.push_back(text("  (empty)"));

      auto teams = hbox({vbox(std::move(p_lines)) | border,
                         vbox(std::move(e_lines)) | border});

      auto msg = text(message_) | center;

      Element action_panel;
      if (current_phase_ == Phase::BattleOver) {
        action_panel = text("Press any key to exit.") | center | bold;
      } else if (current_phase_ == Phase::ChooseAction) {
        int idx = turn_order_[current_turn_idx_];
        bool is_players_choice =
            std::find(player_team_.begin(), player_team_.end(),
                      all_fighters_[idx]) != player_team_.end();
        std::string turn_name = all_fighters_[idx]->name();
        std::string flowers_left = "healing flowers: " + std::to_string(flowers_);
        std::string choice_text = (is_players_choice) ? " (your choice)" : " (enemy's choice)";
        std::string ability1_text = "Ability 1: " + all_fighters_[idx]->getAbility(0).name_ + " - " +
            all_fighters_[idx]->getAbility(0).description;
        std::string ability2_text = "Ability 2: " + all_fighters_[idx]->getAbility(1).name_ + " - " +
            all_fighters_[idx]->getAbility(1).description;
        action_panel =
            vbox({text("Turn: " + turn_name + choice_text) | bold,
                  action_menu_->Render(), separator(), text(flowers_left), paragraph(ability1_text),
                  paragraph(ability2_text)});
      } else if (current_phase_ == Phase::ChooseTarget) {
        action_panel =
            vbox({text("Choose target:") | bold, target_menu_->Render()});
      }

      return vbox({teams, separator(), msg, separator(), action_panel}) |
             border;
    });

    main_component_ = main_renderer | CatchEvent([this](Event event) {
                        if (current_phase_ == Phase::BattleOver) {
                          return true;
                        }

                        int fighter_idx = turn_order_[current_turn_idx_];
                        if (!is_player_team_[fighter_idx]) {
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
