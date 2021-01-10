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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstring>

namespace vkl {
  /**
   * @brief A templating class to define a list of buffer
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

    inline T& data(int i) { return m_uniformBuffers.at(i).data(); }
    inline const T& data(int i) const { return m_uniformBuffers.at(i).data(); }

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

  protected:
    std::deque<Buffer<T>> m_uniformBuffers;

    const Device& m_device;
    const SwapChain& m_swapChain;

    /**
     * @brief Create uniform buffers according to the swapchain's image count
     */
    void createUniformBuffers() {
      const VkDeviceSize bufferSize = sizeof(T);

      m_uniformBuffers.clear();

      for (size_t i = 0; i < m_swapChain.numImages(); i++) {
        m_uniformBuffers.emplace_back(m_device, T(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      }
    }
  };

  /*
   * Specification
   */

  static glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);

  class MVPUniformBuffers : public UniformBuffers<vkl::MVP> {
  public:
    MVPUniformBuffers(const Device& device, const SwapChain& swapChain) : UniformBuffers(device, swapChain) {}

    void update(float time, uint32_t currentImage) {
      MVP& ubo = m_uniformBuffers.at(currentImage).data();

      ubo.model = glm::mat4(1.0f);
      ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

      const float aspect = m_swapChain.extent().width / (float)m_swapChain.extent().height;
      ubo.proj           = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);

      // m_ubo.model = glm::mat4(1.0f);
      // m_ubo.view  = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
      // m_ubo.proj  = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);

      ubo.proj[1][1] *= -1;

      lightPos = glm::vec3(glm::vec4(1)
                           * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

      ubo.lightPos = lightPos;

      void* data;
      vkMapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory(), 0, sizeof(ubo), 0, &data);
      memcpy(data, &ubo, sizeof(ubo));
      vkUnmapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory());
    }
  };

  class DepthUniformBuffers : public UniformBuffers<vkl::Depth> {
  public:
    DepthUniformBuffers(const Device& device, const SwapChain& swapChain) : UniformBuffers(device, swapChain) {}

    void update(float time, uint32_t currentImage) {
      Depth& ubo = m_uniformBuffers.at(currentImage).data();

      // Keep depth range as small as possible
      // for better shadow map precision
      float zNear = 0.1f;
      float zFar  = 10.0f;

      float lightFOV = 45.0f;

      // Matrix from light's point of view
      glm::mat4 depthModel = glm::mat4(1.0f);
      glm::mat4 depthView  = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
      glm::mat4 depthProj  = glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar);

      depthProj[1][1] *= -1;

      ubo.depthMVP = depthProj * depthView * depthModel;

      void* data;
      vkMapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory(), 0, sizeof(ubo), 0, &data);
      memcpy(data, &ubo, sizeof(ubo));
      vkUnmapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory());
    }
  };

}  // namespace vkl

#endif  // VERTEX_HPP