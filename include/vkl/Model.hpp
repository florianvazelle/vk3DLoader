#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <vkl/Vertex.hpp>

namespace vkl {

  class Model {
  public:
    Model(const std::string& modelPath);

    inline const std::vector<Vertex>& vertices() const { return m_vertices; }

  private:
    std::vector<Vertex> m_vertices;
  };

}  // namespace vkl

#endif  // MODEL_HPP