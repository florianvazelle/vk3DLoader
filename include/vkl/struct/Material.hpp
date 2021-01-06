#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>

namespace vkl {

  struct alignas(16) Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
  };

}  // namespace vkl

#endif  // MATERIAL_HPP