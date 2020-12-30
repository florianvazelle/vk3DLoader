#ifndef UNIFORMBUFFER_HPP
#define UNIFORMBUFFER_HPP

#include <vulkan/vulkan.h>
#include <deque>
#include <stdexcept>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/buffer/Buffer.hpp>

namespace vkl {
  /**
   * @brief Liste de buffer uniform
   *
   * Note Exposé : Comme des frames peuvent être "in flight" pendant que nous essayons de modifier
   * le contenu du buffer, nous allons avoir besoin de plusieurs buffers. Nous pouvons soit en avoir
   * un par frame, soit un par image de la swap chain.
   */
  class UniformBuffers : public NonCopyable {
  public:
    UniformBuffers(const Device& device, const SwapChain& swapChain);

    inline const VkBuffer& buffer(int index) const { return m_uniformBuffers[index].buffer(); }

    /**
     * @brief Génére une rotation à chaque frame pour que la géométrie tourne sur elle-même
     * @param currentImage C'est la frame courante
     */
    void update(uint32_t currentImage);

    /**
     * @brief Recreate all uniform buffers when swapchain is recreate
     */
    void recreate();

  private:
    std::deque<Buffer> m_uniformBuffers;

    const Device& m_device;
    const SwapChain& m_swapChain;

    /**
     * @brief Create uniform buffers according to the swapchain's image count
     */
    void createUniformBuffers();
  };

}  // namespace vkl

#endif  // VERTEX_HPP