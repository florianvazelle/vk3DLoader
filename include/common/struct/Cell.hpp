#ifndef CELL_HPP
#define CELL_HPP

#include <glm/glm.hpp>

namespace vkl {

  struct Cell {
    alignas(8) glm::vec2 vel;  // velocity
    alignas(4) float mass;
  };

}  // namespace vkl

#endif  // CELL_HPP