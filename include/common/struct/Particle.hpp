#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <common/VulkanHeader.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

namespace vkl {
  // Pour plus de commentaire regarder Vertex.hpp
  struct Particle {
    glm::vec2 pos;  // position "vec2" because this mpm example works in 2D
    glm::vec2 vel;  // velocity
    glm::mat2 C;    // affine momentum matrix
    float mass;
    float volume_0;  // initial volume

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
              .format   = VK_FORMAT_R32G32_SFLOAT,
              .offset   = offsetof(Particle, pos),
          },
          // velocity
          {
              .location = 1,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32_SFLOAT,
              .offset   = offsetof(Particle, vel),
          },
          {
              .location = 2,
              .binding  = 0,
              .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
              .offset   = offsetof(Particle, C),
          },
          {
              .location = 3,
              .binding  = 0,
              .format   = VK_FORMAT_R32_SFLOAT,
              .offset   = offsetof(Particle, mass),
          },
          {
              .location = 4,
              .binding  = 0,
              .format   = VK_FORMAT_R32_SFLOAT,
              .offset   = offsetof(Particle, volume_0),
          },
      };

      return attributeDescriptions;
    }
  };

}  // namespace vkl

#endif  // PARTICLE_HPP