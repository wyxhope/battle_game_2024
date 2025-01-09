#include "battle_game/core/bullets/cannon_ball.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
CannonBall::CannonBall(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity,
                       int type)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity), type_(type), max_speed_(glm::length(velocity)) {
}

void CannonBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  if(type_ == 0 || type_ == 1){
    SetColor(game_core_->GetNormalBulletColor(player_id_));
  }else{
    SetColor(game_core_->GetSpecialBulletColor(player_id_));
  }
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle3.png");
  DrawModel(0);
}

void CannonBall::Update() {
  position_ += velocity_ * kSecondPerTick;
  if(type_ == 1){
    float k = 0.009f;
    velocity_ += -k * velocity_;
  }
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_* (glm::length(velocity_) / max_speed_) * 10.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

CannonBall::~CannonBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
