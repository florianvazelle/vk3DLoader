#ifndef DEPTH_HPP
#define DEPTH_HPP

#include <poike/poike.hpp>

namespace vkl {

  struct alignas(16) Depth {
    glm::mat4 depthMVP;
  };

}  // namespace vkl

#endif  // DEPTH_HPP