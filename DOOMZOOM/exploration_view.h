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
                  std::vector<Entity>& flowers)
      : map_(map),
        player_(player),
        camera_(camera),
        active_(active),
        in_reserve_(in_reserve),
        entities_(entities),
        flowers_(flowers) {
    map_.loadFromFile(kMapInfo);
  }

  Component getComponent(ScreenInteractive& screen) {
    buildUI(screen);
    return main_component_;
  }
  void setOnCollision(std::function<void(int)> callback) {
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
  void moveEntities() {
    std::uniform_int_distribution<int> dir_dist(0, kDirChar.size() - 1);
    std::discrete_distribution<int> is_moving(
        {kEntityMoveChance, 100 - kEntityMoveChance});
    std::mt19937 engine(std::random_device{}());
    for (auto& entity : entities_) {
      if (is_moving(engine)) {
        int dir = dir_dist(engine);
        int new_x = entity.x + kDirX[dir];
        int new_y = entity.y + kDirY[dir];
        if (map_.isWalkable(new_x, new_y)) {
          if (new_x != player_.x() || new_y != player_.y()) {
            entity.x = new_x;
            entity.y = new_y;
          }
        }
      }
    }
  }

  int checkEnemyCollision() const {
    for (int i = 0; i < entities_.size(); ++i) {
      if (entities_[i].x == player_.x() && entities_[i].y == player_.y()) {
        return i;
      }
    }

    return -1;
  }

  int checkFlowerCollision() const {
    for (int i = 0; i < flowers_.size(); ++i) {
      if (flowers_[i].x == player_.x() && flowers_[i].y == player_.y()) {
        return i;
      }
    }

    return -1;
  }

  void buildUI(ScreenInteractive& screen) {
    auto map_renderer = Renderer([&] {
      int terminal_w = screen.dimx();
      int terminal_h = screen.dimy();
      int view_w = std::max(1, terminal_w - kCameraDX);
      int view_h = std::max(1, terminal_h - kCameraDY);
      auto [camX, camY] = camera_.getOffset(view_w, view_h);

      Elements rows;
      for (int row = 0; row < view_h; ++row) {
        std::string line(view_w, ' ');
        for (int col = 0; col < view_w; ++col) {
          int wx = camX + col;
          int wy = camY + row;

          bool is_entity = false;
          bool is_flower = false;
          for (auto& entity : entities_) {
            if (entity.x == wx && entity.y == wy) {
              is_entity = true;
              break;
            }
          }

          for (auto& entity : flowers_) {
            if (entity.x == wx && entity.y == wy) {
              is_flower = true;
              break;
            }
          }

          if (wy >= 0 && wy < map_.height() && wx >= 0 && wx < map_.width()) {
            if (wx == player_.x() && wy == player_.y())
              line[col] = '@';
            else if (is_entity)
              line[col] = 'W';
            else if (is_flower)
              line[col] = '*';
            else
              line[col] = map_.at(wx, wy);
          }
        }

        rows.push_back(text(line));
      }
      return vbox(std::move(rows)) | border | center;
    });

    auto global_handler = CatchEvent([this](Event event) {
      if (event.is_character()) {
        char key = std::tolower(event.character()[0]);

        auto it = std::find(kDirChar.begin(), kDirChar.end(), key);
        if (it == kDirChar.end()) {
          return false;
        } else {
          int index = std::distance(kDirChar.begin(), it);
          player_.move(kDirX[index], kDirY[index]);
          int idx = checkFlowerCollision();
          if (idx != -1) {
            player_.addFlower();
            flowers_.erase(flowers_.begin() + idx);
          }

          moveEntities();
          idx = checkEnemyCollision();
          if (idx != -1 && collision_) {
            collision_(idx);
          }

          return true;
        }
      }

      return false;
    });

    // команда (пойдёт в бой)
    MenuOption active_opt;
    active_opt.on_enter = [&] { active_.transferTo(in_reserve_, kTeamMin); };
    auto active_menu =
        Menu(&active_.names(), &active_.selectedRef(), active_opt);
    auto active_column = Renderer(active_menu, [=] {
      return vbox({
          text("On team (max 3)") | bold,
          separator(),
          active_menu->Render() | border,
      });
    });

    // вектор резерва
    MenuOption reserve_opt;
    reserve_opt.on_enter = [&] {
      in_reserve_.transferTo(active_, 0, kTeamMax);
    };
    auto reserve_menu =
        Menu(&in_reserve_.names(), &in_reserve_.selectedRef(), reserve_opt);
    auto reserve_column = Renderer(reserve_menu, [=] {
      return vbox({
          text("Waiting at zoo") | bold,
          separator(),
          reserve_menu->Render() | size(HEIGHT, LESS_THAN, kInReserveVBoxHeight) | border,
      });
    });

    auto selected_animal = Renderer([=] {
      return vbox({
          separator(),
          paragraph(active_.getDescription()) | bold,
          separator(),
          paragraph(in_reserve_.getDescription()) | bold,
      });
    });

    auto container = Container::Horizontal({
        map_renderer,
        Container::Vertical({active_column, reserve_column, selected_animal}),
    });

    auto final_renderer = Renderer(container, [=] {
      return hbox({
                 map_renderer->Render() | flex,
                 separator(),
                 vbox({active_column->Render(), reserve_column->Render(),
                       selected_animal->Render(),
                       text("healing flowers: " +
                            std::to_string(player_.getFlowers()))}) |
                     size(WIDTH, EQUAL, kTeamVBoxWidth),
             }) |
             border;
    });

    main_component_ = final_renderer | global_handler;
  }
};