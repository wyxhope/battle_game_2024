#include "wyxhope_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include <iostream>
#include <cmath>

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

wyxhope_Tank::wyxhope_Tank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      tank_body_model_index = mgr->RegisterModel(
          {
              {{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              // distinguish front and back
              {{0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 1, 2, 3, 0, 2, 5, 2, 4, 5});
    }

    {
      /* Tank Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void wyxhope_Tank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void wyxhope_Tank::Update() {
  switchmode();
  if(tank_mode_ == false){
    TankMove(3.0f, glm::radians(180.0f));
  }else{
    TankMove(6.0f, glm::radians(200.0f));
  }
  TurretRotate();
  Fire();

}

void wyxhope_Tank::TankMove(float move_speed, float rotate_angular_speed) {
  //Add acceleration process
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += (current_speed_ / move_speed) * 1.0f;
      if(current_speed_ <= move_speed * 9.1 / 10){
        current_speed_ += move_speed / 10;
      }
      if (current_speed_ > move_speed * 10.1 / 10) {
        current_speed_ -= move_speed / 20;
      }
    } else if (input_data.key_down[GLFW_KEY_S]) {
      offset.y += (current_speed_ / move_speed) * 1.0f;
      if(current_speed_ >= -move_speed * 9.1 / 10){
        current_speed_ -= move_speed / 10;
      }
      if(current_speed_ < -move_speed * 10.1 / 10){
        current_speed_ += move_speed / 20;
      }
    } else {
      offset.y += (current_speed_ / move_speed) * 1.0f;
      if(abs(current_speed_) < move_speed / 50){
        current_speed_ = 0;
      }else if(current_speed_ > 0){
        current_speed_ -= move_speed / 20;
      }else if(current_speed_ < 0){
        current_speed_ += move_speed / 20;
      }
    }
    float speed = std::abs(current_speed_) * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
      if(rotate_speed_ <= 9.1 * rotate_angular_speed / 10){
        rotate_speed_ += rotate_angular_speed / 10;
      }
    } else if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset += 1.0f;
      if(rotate_speed_ >= -9.1 * rotate_angular_speed / 10){
        rotate_speed_ -= rotate_angular_speed / 10;
      }
    } else {
      rotation_offset += 1.0f;
      if(std::abs(rotate_speed_ <= rotate_angular_speed / 50)){
        rotate_speed_ = 0;
      }else if(rotate_speed_ > 0){
        rotate_speed_ -= rotate_angular_speed / 20;
      }else if(rotate_speed_ < 0){
        rotate_speed_ += rotate_angular_speed / 20;
      }
    }
    rotation_offset *= kSecondPerTick * rotate_speed_ * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void wyxhope_Tank::switchmode(){
  auto player = game_core_->GetPlayer(player_id_);
  if(player){
    auto &input_data = player->GetInputData();
    if (tank_mode_ == true && quick_mode_time_ >= 300){
      tank_mode_ = false;
      quick_mode_time_ = 0;
      return;
    } else if(tank_mode_ == true){
      quick_mode_time_++;
    }
    if (input_data.key_down[GLFW_KEY_Q] && switch_count_down_ >= 120) {
      if (tank_mode_ == false) {
        tank_mode_ = true;
      } else {
        tank_mode_ = false;
      }
      switch_count_down_ = 0;
      quick_mode_time_ = 0;
    }else{
      switch_count_down_++;
    }
    if(input_data.key_down[GLFW_KEY_E]){
      if(fire_mode_ == false){
        fire_mode_ = true;
      }else{
        fire_mode_ = false;
      }
    }
  }
}


void wyxhope_Tank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void wyxhope_Tank::Fire() {
  if (fire_count_down_ == 0 && fire_mode_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        if(fire_mode_ == false){
          int random = game_core_->RandomInt(1, 100);
        // with probability 10% the tank will generate a bullet with 2*speed and 2*damage, the color is red, otherwise 
        // the bullet will be normal
          if(random <= 10){
            auto velocity = Rotate(glm::vec2{0.0f, 40.0f}, turret_rotation_);
            int type = 2;
            GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, 4.0 * GetDamageScale(), velocity, type);
            fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
            consecutive_fire_count_++;
          }else{
          //when the tank is moving, it's harder to accurately hit the target
            auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
            int type = 1;
            GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, 2.2 * GetDamageScale(), velocity, type);
            fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
            consecutive_fire_count_++;
          }
          if(consecutive_fire_count_ >= 6){
            fire_count_down_ = 2 * kTickPerSecond;
            consecutive_fire_count_ = 0;
          }
        }else{
          auto velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_);
          GenerateBullet<bullet::Rebounce_ball>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, 6.0 * GetDamageScale(), velocity);
          fire_mode_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        }
      }else{
        consecutive_fire_count_ = 0;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
  if (fire_mode_count_down_){
    fire_mode_count_down_--;
  }
}


bool wyxhope_Tank::IsHit(glm::vec2 position) const {
  //modify the hitbox of the tank
  position = WorldToLocal(position);
  return position.x > -1.0f && position.x < 1.0f && position.y > -1.3f &&
         position.y < 1.3f && position.x + position.y < 1.9f &&
         position.y - position.x < 1.9f;
}

const char *wyxhope_Tank::UnitName() const {
  return "wyx Tank";
}

const char *wyxhope_Tank::Author() const {
  return "wyx";
}
}  // namespace battle_game::unit
