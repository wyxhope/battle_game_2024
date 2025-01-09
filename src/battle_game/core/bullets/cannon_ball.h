#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class CannonBall : public Bullet {
 public:
  CannonBall(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity,
             int type);
  ~CannonBall() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float max_speed_{};
  int type_{0};//type 0 is normal, type 1 is special
};
}  // namespace battle_game::bullet
