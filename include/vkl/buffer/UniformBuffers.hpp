#ifndef UNIFORMBUFFER_HPP
#define UNIFORMBUFFER_HPP

#include <vulkan/vulkan.h>
#include <deque>
#include <stdexcept>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/buffer/Buffer.hpp>
#include <vkl/struct/Depth.hpp>
#include <vkl/struct/MVP.hpp>

namespace vkl {
  /**
   * @brief Liste de buffer uniform
   *
   * Note Exposé : Comme des frames peuvent être "in flight" pendant que nous essayons de modifier
   * le contenu du buffer, nous allons avoir besoin de plusieurs buffers. Nous pouvons soit en avoir
   * un par frame, soit un par image de la swap chain.
   */
  template <typename T> class UniformBuffers : public NonCopyable {
  public:
    UniformBuffers(const Device& device, const SwapChain& swapChain) : m_device(device), m_swapChain(swapChain) {
      createUniformBuffers();
    }

    inline const VkBuffer& buffer(int index) const { return m_uniformBuffers[index].buffer(); }

    /**
     * @brief Génére une rotation à chaque frame pour que la géométrie tourne sur elle-même
     * @param currentImage C'est la frame courante
     */
    void update(float time, uint32_t currentImage) { throw std::logic_error("Update method not implemented!"); }

    /**
     * @brief Recreate all uniform buffers when swapchain is recreate
     */
    void recreate() {
      m_uniformBuffers.clear();  // -> call destructor of all Buffer
      createUniformBuffers();
    }

    inline T& ubo() { return m_ubo; }
    inline const T& ubo() const { return m_ubo; }

  private:
    T m_ubo;
    std::deque<Buffer> m_uniformBuffers;

    const Device& m_device;
    const SwapChain& m_swapChain;

    /**
     * @brief Create uniform buffers according to the swapchain's image count
     */
    void createUniformBuffers() {
      const VkDeviceSize bufferSize = sizeof(T);

      m_uniformBuffers.clear();

      // m_uniformBuffers.reserve(m_swapChain.numImages());
      for (size_t i = 0; i < m_swapChain.numImages(); i++) {
        m_uniformBuffers.emplace_back(m_device);
      }

      for (size_t i = 0; i < m_swapChain.numImages(); i++) {
        m_uniformBuffers.at(i).createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      }
    }
  };  // namespace vkl

  template <> void UniformBuffers<vkl::MVP>::update(float time, uint32_t currentImage);
  template <> void UniformBuffers<vkl::Depth>::update(float time, uint32_t currentImage);

}  // namespace vkl

#endif  // VERTEX_HPP