#ifndef UBO_HPP
#define UBO_HPP

#include <glm/glm.hpp>

#include <vkl/Device.hpp>

namespace vkl {

  struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
  };

}  // namespace vkl

#endif  // UBO_HPP