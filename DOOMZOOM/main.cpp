#include <game_manager.h>
int main() {
  auto screen = ScreenInteractive::Fullscreen();
  GameManager game(screen);
  game.run();
  return 0;
}