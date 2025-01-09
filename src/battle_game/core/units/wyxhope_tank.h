#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class wyxhope_Tank : public Unit {
 public:
  wyxhope_Tank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  
 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void switchmode();
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t consecutive_fire_count_{0};
  uint32_t mine_count_down_{0};
  float current_speed_{0.0f};
  float rotate_speed_{0.0f};
  bool tank_mode_{0};
  uint32_t switch_count_down_{0};
  uint32_t quick_mode_time_{0};
};
}  // namespace battle_game::unit
