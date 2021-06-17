#ifndef PARTICLEMVP_HPP
#define PARTICLEMVP_HPP

#include <glm/glm.hpp>
#include <common/struct/MVP.hpp>

namespace vkl {

  struct ParticleMVP : public MVP {
    glm::vec2 screenDim;
  };

}  // namespace vkl

#endif  // PARTICLEMVP_HPP
