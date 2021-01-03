#include <vkl/Model.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <stdexcept>

using namespace vkl;

bool hasEnding(std::string const& fullString, std::string const& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

Model::Model(const std::string& modelPath) {
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

    for (const auto& shape : shapes) {
      for (const auto& index : shape.mesh.indices) {
        Vertex vertex{};

        vertex.pos = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        vertex.texCoord = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
        };

        vertex.color = {1.0f, 1.0f, 1.0f};

        m_vertices.push_back(vertex);
      }
    }
  } else {
    // By default show a triangle
    const std::vector<Vertex> triangle = {
        {
            {0.0f, -0.5f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f},
        },
        {
            {0.5f, 0.5f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f},
        },
        {
            {-0.5f, 0.5f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f},
        },
    };

    m_vertices = triangle;
  }
}