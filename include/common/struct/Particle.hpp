#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <common/VulkanHeader.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace vkl {
  // Pour plus de commentaire regarder Vertex.hpp
  struct Particle {
    glm::vec4 pos;  // xyz = position, w = mass
    glm::vec4 vel;  // xyz = velocity, w = gradient texture position

    static VkVertexInputBindingDescription getBindingDescription() {
      // Binding description
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding   = 0;
      bindingDescription.stride    = sizeof(Particle);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
      // Attribute descriptions
      // Describes memory layout and shader positions
      std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
          // position
          {
              .location = 0,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
              .offset   = offsetof(Particle, pos),
          },
          // velocity
          {
              .location = 1,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
              .offset   = offsetof(Particle, vel),
          },
      };

      return attributeDescriptions;
    }
  };

}  // namespace vkl

#endif  // PARTICLE_HPP