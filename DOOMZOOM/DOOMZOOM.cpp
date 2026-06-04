#include <game_manager.h>
int main() {
  auto screen = ScreenInteractive::Fullscreen();

  //std::vector<std::shared_ptr<Animal>> player_team = {
  //    std::make_shared<Animal>(0),
  //    std::make_shared<Animal>(4)
  //};

  //std::vector<std::shared_ptr<Animal>> enemy_team = {
  //    std::make_shared<Animal>(3),
  //    std::make_shared<Animal>(1)
  //};

  //BattleView battle(player_team, enemy_team, screen);
  //screen.Loop(battle.getComponent());


  GameManager game(screen);
  game.run();
  return 0;
}