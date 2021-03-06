// clang-format off
#include <common/Model.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>           // for material_t, attrib_t, (anonymous)

#include <iostream>                    // for endl, basic_ostream, cerr, ost...
#include <stdexcept>                   // for runtime_error
#include <common/struct/Material.hpp>  // for Material, vkl
#include <common/struct/Vertex.hpp>    // for Vertex
#include <glm/glm.hpp>
// clang-format on

using namespace vkl;

bool hasEnding(std::string const& fullString, std::string const& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

Model::Model(const Device& device, const CommandPool& commandPool, const std::string& modelPath) {
  if (hasEnding(modelPath, ".obj")) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str());

    if (!err.empty()) {
      std::cerr << err << std::endl;
    }

    if (!ret) {
      throw std::runtime_error("failed to load : " + modelPath);
    }

    if (shapes.size() == 0) {
      throw std::runtime_error("err: # of shapes are zero.");
    }

    bool hasNormals   = !attrib.normals.empty();
    bool hasTexCoords = !attrib.texcoords.empty();

    for (const auto& shape : shapes) {
      for (const auto& index : shape.mesh.indices) {
        Vertex vertex{};

        vertex.pos = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        if (hasNormals) {
          vertex.normal = {
              attrib.normals[3 * index.normal_index + 0],
              attrib.normals[3 * index.normal_index + 1],
              attrib.normals[3 * index.normal_index + 2],
          };
        }

        vertex.color = {1.0f, 1.0f, 1.0f};

        if (hasTexCoords) {
          vertex.texCoord = {
              attrib.texcoords[2 * index.texcoord_index + 0],
              1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
          };
        }

        m_vertices.push_back(vertex);
      }
    }

    for (const tinyobj::material_t& material : materials) {
      Material m = {
          {material.ambient[0], material.ambient[1], material.ambient[2]},
          {material.diffuse[0], material.diffuse[1], material.diffuse[2]},
          {material.specular[0], material.specular[1], material.specular[2]},
          material.shininess,
      };

      m_materials.push_back(m);

      if (material.diffuse_texname.length() > 0) {
          m_textures.push_back(std::make_unique<Texture>(device, commandPool, material.diffuse_texname));
      }
    }

    if (m_textures.size() == 0) {
        m_textures.push_back(std::make_unique<Texture>(device, commandPool, "assets/textures/blank.png"));
    }
  }
}
