#ifndef UBO_HPP
#define UBO_HPP

#include <glm/glm.hpp>

namespace vkl {

  struct alignas(16) MVP {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 depthBiasMVP;
    glm::vec3 lightPos;
  };

}  // namespace vkl

#endif  // UBO_HPP