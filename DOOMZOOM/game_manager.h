#include <battle_view.h>
#include <exploration_view.h>

class GameManager {
 public:
  GameManager(ScreenInteractive& screen);
  void run();

 private:
  ScreenInteractive& screen_;
  Map map_;
  Player player_;
  Camera camera_;
  Inventory active_;
  Inventory in_reserve_;
  Component main_component_;
  std::vector<Entity> entities_;
  std::vector<Entity> flowers_;
  int next_enemy_idx = 2;
  int collision_entity_index_ = -1;
  State state_ = Exploration;
  ExplorationView exploration_;
  std::unique_ptr<BattleView> battle_;

  void endBattle();
  void initEntities();
  void startBattle(int entity_index);
};