#pragma once

#include <camera.h>
#include <inventory.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

using namespace ftxui;

struct Entity {
  int x;
  int y;
};

class ExplorationView {
 public:
  ExplorationView(Map& map, Player& player, Camera& camera, Inventory& active,
                  Inventory& in_reserve, std::vector<Entity>& entities,
                  std::vector<Entity>& flowers);

  Component getComponent(ScreenInteractive& screen);
  inline void setOnCollision(std::function<void(int)> callback) {
    collision_ = callback;
  }

 private:
  Map& map_;
  Player& player_;
  Camera& camera_;
  Inventory& active_;
  Inventory& in_reserve_;
  Component main_component_;
  std::vector<Entity>& entities_;
  std::vector<Entity>& flowers_;
  std::function<void(int)> collision_;

  
  void buildUI(ScreenInteractive& screen);
  int checkEnemyCollision() const;
  int checkFlowerCollision() const;
  void moveEntities();
};