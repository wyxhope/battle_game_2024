#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Rebounce_ball : public Bullet {
 public:
  Rebounce_ball(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~Rebounce_ball() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float max_speed_{};
};
}