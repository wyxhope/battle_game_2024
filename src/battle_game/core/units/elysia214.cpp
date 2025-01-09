#include "elysia214.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

const int CHARGE=0, READY=1, BOOST=2;

Elysia214::Elysia214(GameCore *game_core, uint32_t id, uint32_t player_id)
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
      backround_engbar_color_={1.0f, 0.0f, 0.0f, 0.9f};
      charging_front_engbar_color_={0.0f, 0.0f, 1.0f, 0.9f};
      charged_front_engbar_color_={1.0f, 1.0f, 0.0f, 0.9f};
      boosted_engbar_color_={1.0f, 0.5f, 0.0f, 0.9f};
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

void Elysia214::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(glm::vec4{1.0f, 0.45f, 1.0f, 1.0f});
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

//void Elysia214::RenderHelper() {
  
    
    /*auto parent_unit = game_core_->GetUnit(id_);
    auto pos = parent_unit->GetPosition() + lifebar_offset_;
    pos += glm::vec2{0.0f, 0.2f};

  float energy_percentage = energy_ / (10.0f*kTickPerSecond);
  glm::vec2 shift_eng = {(float)lifebar_length_ * (1 - energy_percentage) / 2, 0.0f};


  SetTransformation(pos, 0.0f, {lifebar_length_, 1.0f});
  SetColor(backround_engbar_color_); 
  SetTexture(0);*/
  //DrawModel(life_bar_model_index);

  
  /*SetTransformation(pos - shift_eng, 0.0f, {lifebar_length_ * energy_percentage, 1.0f});
  if(status==CHARGE){
    SetColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});

  }else{
  SetColor(glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});  
  }
  DrawModel(life_bar_model_index);*/
  
//}



void Elysia214::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void Elysia214::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
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
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void Elysia214::TurretRotate() {
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

void Elysia214::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        if(status==BOOST){
          for(int i=-1;i<=1;i++){
            auto velocity = Rotate(glm::vec2{0.0f, 30.0f}, turret_rotation_ + glm::radians(30.0f * i));
            GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + glm::radians(30.0f * i)),
                turret_rotation_ + glm::radians(30.0f * i), 1.0f * GetDamageScale(), velocity, 0);
          }
        fire_count_down_ = 0.8f * kTickPerSecond;  // Fire interval 1 second.
        
      }else{
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 0.75f * GetDamageScale(), velocity, 0);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        if (status==CHARGE){
          energy_+=kTickPerSecond;
        }
      }
      }
      if(input_data.key_down[GLFW_KEY_Q]&& status==READY){
      status=BOOST;
    }
      
    }
  }
  if(status==CHARGE){
    energy_+=0.5f;
    if(energy_>=10.0f * kTickPerSecond){
      energy_=10.0f * kTickPerSecond;
      status=READY;
    }
  }else if (status==READY){
    energy_=10.0f * kTickPerSecond;
    
  }else{
    energy_-=2.0f;
    if(energy_<=0.0f){
      energy_=0.0f;
      status=CHARGE;
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool Elysia214::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *Elysia214::UnitName() const {
  return "Elysia";
}

const char *Elysia214::Author() const {
  return "zmh24";
}
}  // namespace battle_game::unit
