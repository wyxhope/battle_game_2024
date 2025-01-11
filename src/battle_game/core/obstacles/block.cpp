#include "battle_game/core/obstacles/block.h"
#include <cmath>

namespace battle_game::obstacle {

Block::Block(GameCore *game_core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

bool Block::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

std::pair<glm::vec2, glm::vec2> Block::GetSurfaceNormal(glm::vec2 origin,
                                                        glm::vec2 terminus) {
  auto local_terminus = WorldToLocal(terminus);
  float x_left = std::abs(local_terminus.x + scale_.x);
  float x_right = std::abs(local_terminus.x - scale_.x);
  float y_up = std::abs(local_terminus.y + scale_.y);
  float y_down = std::abs(local_terminus.y - scale_.y);
  float min_value = std::min(std::min(x_left, x_right), std::min(y_up, y_down));
  if(x_left == min_value){
    return std::make_pair(terminus, glm::vec2{-1.0f * glm::cos(rotation_), -1.0f * glm::sin(rotation_)});
  }else if(x_right == min_value){
    return std::make_pair(terminus, glm::vec2{1.0f * glm::cos(rotation_), 1.0f * glm::sin(rotation_)});
  }else if(y_up == min_value){
    return std::make_pair(terminus, glm::vec2{-1.0f * glm::sin(rotation_), 1.0f * glm::cos(rotation_)});
  }else if(y_down == min_value){
    return std::make_pair(terminus, glm::vec2{1.0f * glm::sin(rotation_), -1.0f * glm::cos(rotation_)});
  }
  return std::make_pair(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
}

void Block::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
