#include <battle_view.h>
#include <exploration_view.h>

class GameManager {
 public:
  explicit GameManager(ScreenInteractive& screen)
      : map_(kMapInfo),
        player_(map_),
        camera_(map_, player_),
        active_({std::make_shared<Animal>(0), std::make_shared<Animal>(1)}),
        in_reserve_({}),
        screen_(screen) {
    initEntities();

    exploration_ = std::make_unique<ExplorationView>(
        map_, player_, camera_, active_, in_reserve_, entities_, flowers_);
    exploration_->setOnCollision([this](int idx) { startBattle(idx); });
  }

  void run() {
    auto exploration_component = exploration_->getComponent(screen_);

    main_component_ = Renderer([this, exploration_component] {
      if (state_ == Exploration)
        return exploration_component->Render();
      else if (state_ == Battle && battle_)
        return battle_->getComponent()->Render();
      return text("You Won! Thank you for playing! <3") | center;
    });

    main_component_ = main_component_ |
                      CatchEvent([this, exploration_component](Event event) {
                        if (state_ == State::GameOver) {
                          screen_.ExitLoopClosure()();
                          return true;
                        }

                        if (event.is_character() &&
                            std::tolower(event.character()[0]) == 'q') {
                          screen_.ExitLoopClosure()();
                          return true;
                        }

                        if (state_ == Exploration) {
                          return exploration_component->OnEvent(event);
                        } else if (state_ == Battle && battle_) {
                            bool handled =
                              battle_->getComponent()->OnEvent(event);
                          if (event.is_character() &&
                              std::tolower(event.character()[0]) == 'e' ||
                                battle_->isOver()) {
                            endBattle();
                          }

                          return handled;
                        }
                        return false;
                      });

    screen_.Loop(main_component_);
  }

 private:
  ScreenInteractive& screen_;
  Map map_;
  Player player_;
  Camera camera_;
  Inventory active_;
  Inventory in_reserve_;
  std::vector<Entity> entities_;
  std::vector<Entity> flowers_;
  int next_enemy_idx = 2;

  std::unique_ptr<ExplorationView> exploration_;
  std::unique_ptr<BattleView> battle_;

  State state_ = Exploration;
  int collision_entity_index_ = -1;

  Component main_component_;

  void initEntities() {
    entities_.reserve(kMovingEntityAmount);
    std::uniform_int_distribution<int> dist_x(0, map_.width() - 1);
    std::uniform_int_distribution<int> dist_y(0, map_.height() - 1);
    std::mt19937 engine(std::random_device{}());
    for (int i = 0; i < (kFlowersPerBattle + 1) * kMovingEntityAmount; ++i) {
      while (true) {
        int x = dist_x(engine);
        int y = dist_y(engine);
        if (map_.isWalkable(x, y) && (x != player_.x() || y != player_.y())) {
          if (i < kMovingEntityAmount)
            entities_.push_back({x, y});
          else
            flowers_.push_back({x, y});
          break;
        }
      }
    }
  }

  void startBattle(int entity_index) {
    if (active_.team().empty()) return;
    collision_entity_index_ = entity_index;

    Team enemy_team({std::make_shared<Animal>(next_enemy_idx),
                     std::make_shared<Animal>(next_enemy_idx + 1)});

    Team player_team = active_.team();

    battle_ = std::make_unique<BattleView>(player_team, enemy_team, player_.getFlowers());
    state_ = State::Battle;
  }

  void endBattle() {
    bool victory = battle_->isVictory();
    player_.setFlowers(battle_->getFlowersleft());
    battle_.reset();
    for (auto& a : active_.team()) {
      a->clearStatuses();
      a->restoreStats();
      a->heal();
    }

    if (victory) {
      Inventory enemy_team({std::make_shared<Animal>(next_enemy_idx),
                            std::make_shared<Animal>(next_enemy_idx + 1)});
      enemy_team.transferTo(in_reserve_);
      enemy_team.transferTo(in_reserve_);
      
      next_enemy_idx += 2;
      entities_.erase(entities_.begin() + collision_entity_index_);
    } else {
      auto xy_start = map_.findStart();
      player_.setXY(xy_start.first, xy_start.second);
    }

    if (entities_.empty()) state_ = State::GameOver;
    else state_ = State::Exploration;
  }
};