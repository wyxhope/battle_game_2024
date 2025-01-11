#include "battle_game/core/bullets/new_ball.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
#include <iostream>

namespace battle_game::bullet {
Rebounce_ball::Rebounce_ball(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity
                       ): Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity), max_speed_(glm::length(velocity)) {
}

void Rebounce_ball::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(game_core_->GetRebounceBulletColor(player_id_));
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle3.png");
  DrawModel(0);
}

void Rebounce_ball::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsOutOfRange(position_)) {
    auto normal = game_core_->GetSurfaceNormalVector(position_);
    if(normal.x == 1 || normal.x == -1){
        velocity_.x = -velocity_.x;
    }
    if(normal.y == 1 || normal.y == -1){
        velocity_.y = -velocity_.y;
    }
  } else if (game_core_->IsBlockedByObstacles(position_)) {
    auto obstacle = game_core_->GetBlockedObstacle(position_);
    auto normal = obstacle->GetSurfaceNormal(glm::vec2{0.0f, 0.0f}, position_);
    velocity_ = glm::reflect(velocity_, normal.second);
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    /*if (unit.first == unit_id_) {
      continue;
    }
    */
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_* (glm::length(velocity_) / max_speed_) * 10.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

Rebounce_ball::~Rebounce_ball() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}