#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/Vertex.hpp>
#include <vkl/buffer/Buffer.hpp>

namespace vkl {
  class VertexBuffer : public Buffer {
  public:
    VertexBuffer(const Device& device, const std::vector<Vertex>& vertices);

    inline const size_t size() const { return m_vertices.size(); }

  private:
    const std::vector<Vertex>& m_vertices;
  };

}  // namespace vkl

#endif  // VERTEX_HPP