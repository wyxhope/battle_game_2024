#pragma once
#include "battle_game/graphics/util.h"
#include "grassland/grassland.h"

namespace battle_game {
struct DeviceModel {
  std::unique_ptr<vulkan_legacy::framework::StaticBuffer<ObjectVertex>>
      vertex_buffer;
  std::unique_ptr<vulkan_legacy::framework::StaticBuffer<uint32_t>>
      index_buffer;
};
}  // namespace battle_game
