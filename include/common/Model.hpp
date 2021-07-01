/**
 * @file Model.hpp
 * @brief Define Model class
 * @todo Need to refactor it, to support texture, multiple Material ...
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include <common/struct/Material.hpp>
#include <common/image/Texture.hpp>
#include <common/CommandPool.hpp>
#include <common/struct/Vertex.hpp>
#include <string>
#include <vector>
#include <memory>

namespace vkl {

  class Model {
  public:
    Model(const Device& device, const CommandPool& commandPool, const std::string& modelPath);

    inline const std::vector<Vertex>& vertices() const { return m_vertices; }
    inline const std::vector<Material>& materials() const { return m_materials; }
    inline const std::vector<std::unique_ptr<Texture>>& textures() const { return m_textures; }

  private:
    std::vector<Vertex> m_vertices;
    std::vector<Material> m_materials;
    std::vector<std::unique_ptr<Texture>> m_textures;
  };

}  // namespace vkl

#endif  // MODEL_HPP
