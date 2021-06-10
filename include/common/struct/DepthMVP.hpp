#ifndef DEPTHUBO_HPP
#define DEPTHUBO_HPP

#include <common/struct/MVP.hpp>
#include <glm/glm.hpp>

namespace vkl {

  struct alignas(16) DepthMVP : public MVP {
    glm::mat4 depthBiasMVP;
    glm::vec3 lightPos;
  };

}  // namespace vkl

#endif  // DEPTHUBO_HPP