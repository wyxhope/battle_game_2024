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
  if(std::abs(local_terminus.x + scale_.x) < 0.17f){
    return std::make_pair(terminus, glm::vec2{-1.0f, 0.0f});
  }else if(std::abs(local_terminus.x - scale_.x) < 0.17f){
    return std::make_pair(terminus, glm::vec2{1.0f, 0.0f});
  }else if(std::abs(local_terminus.y + scale_.y) < 0.17f){
    return std::make_pair(terminus, glm::vec2{0.0f, -1.0f});
  }else if(std::abs(local_terminus.y - scale_.y) < 0.17f){
    return std::make_pair(terminus, glm::vec2{0.0f, 1.0f});
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
