#ifndef MATERIALBUFFER_HPP
#define MATERIALBUFFER_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/buffer/Buffer.hpp>
#include <vkl/struct/Material.hpp>

namespace vkl {
  class MaterialBuffer : public Buffer {
  public:
    MaterialBuffer(const Device& device, const Material& material);

  private:
    const Material& m_material;
  };

}  // namespace vkl

#endif  // MATERIALBUFFER_HPP