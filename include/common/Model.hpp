/**
 * @file Model.hpp
 * @brief Define Model class
 * @todo Need to refactor it, to support texture, multiple Material ...
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include <common/struct/Material.hpp>
#include <common/struct/Vertex.hpp>
#include <string>
#include <vector>

namespace vkl {

  class Model {
  public:
    Model(const std::string& modelPath);

    inline const std::vector<Vertex>& vertices() const { return m_vertices; }
    inline const std::vector<Material>& materials() const { return m_materials; }

  private:
    std::vector<Vertex> m_vertices;
    std::vector<Material> m_materials;
  };

}  // namespace vkl

#endif  // MODEL_HPP