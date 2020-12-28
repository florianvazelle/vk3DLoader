#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/Vertex.hpp>

namespace vkl {
  class VertexBuffer : public NonCopyable {
  public:
    VertexBuffer(const Device& device, const std::vector<Vertex>& vertices);
    ~VertexBuffer();

    inline const VkBuffer& handle() const { return m_vertexBuffer; }
    inline const size_t size() const { return m_vertices.size(); }

  private:
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;

    const Device& m_device;
    const std::vector<Vertex>& m_vertices;
  };

}  // namespace vkl

#endif  // VERTEX_HPP