#ifndef CELL_HPP
#define CELL_HPP

#include <glm/glm.hpp>

namespace vkl {

  struct Cell {
    glm::vec2 vel;  // velocity
    float mass;
    float padding;  // unused
  };

}  // namespace vkl

#endif  // CELL_HPP
