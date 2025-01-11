#pragma once
#include "battle_game/core/obstacle.h"

namespace battle_game::obstacle {
class Block : public Obstacle {
 public:
  Block(GameCore *game_core,
        uint32_t id,
        glm::vec2 position,
        float rotation = 0.0f,
        glm::vec2 scale = glm::vec2{1.0f, 1.0f});
  std::pair<glm::vec2, glm::vec2> GetSurfaceNormal(glm::vec2 origin,
                                                   glm::vec2 terminus) override;

 private:
  [[nodiscard]] bool IsBlocked(glm::vec2 p) const override;
  void Render() override;
  glm::vec2 scale_{1.0f};
};
}  // namespace battle_game::obstacle
