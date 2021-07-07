#ifndef DEPTHUBO_HPP
#define DEPTHUBO_HPP

#include <glm/glm.hpp>
#include <struct/MVP.hpp>

namespace vkl {

  struct alignas(16) DepthMVP : public MVP {
    glm::mat4 depthBiasMVP;
    glm::vec3 lightPos;
  };

}  // namespace vkl

#endif  // DEPTHUBO_HPP