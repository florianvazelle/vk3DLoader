#pragma once

#include <vulkan/vulkan.h>
#include <common/GraphicsPipeline.hpp>
#include <vector>

namespace vkl {

  namespace misc {

    inline VkSpecializationMapEntry specializationMapEntry(uint32_t constantID, uint32_t offset, size_t size) {
      return {
          .constantID = constantID,
          .offset     = offset,
          .size       = size,
      };
    }

    inline VkSpecializationInfo specializationInfo(uint32_t mapEntryCount,
                                                   const VkSpecializationMapEntry* mapEntries,
                                                   size_t dataSize,
                                                   const void* data) {
      return {
          .mapEntryCount = mapEntryCount,
          .pMapEntries   = mapEntries,
          .dataSize      = dataSize,
          .pData         = data,
      };
    }

  }  // namespace misc

}  // namespace vkl
