#include <vkl/buffer/UniformBuffers.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <cstring>
#include <vkl/UniformBufferObject.hpp>

using namespace vkl;

UniformBuffers::UniformBuffers(const Device& device, const SwapChain& swapChain)
    : m_device(device), m_swapChain(swapChain) {
  createUniformBuffers();
}

void UniformBuffers::recreate() {
  m_uniformBuffers.clear();  // -> call destructor of all Buffer
  createUniformBuffers();
}

void UniformBuffers::createUniformBuffers() {
  const VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  m_uniformBuffers.clear();

  // m_uniformBuffers.reserve(m_swapChain.numImages());
  for (size_t i = 0; i < m_swapChain.numImages(); i++) {
    m_uniformBuffers.emplace_back(m_device);
  }

  for (size_t i = 0; i < m_swapChain.numImages(); i++) {
    m_uniformBuffers[i].createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
}

void UniformBuffers::update(uint32_t currentImage) {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  ubo.proj = glm::perspective(glm::radians(45.0f), m_swapChain.extent().width / (float)m_swapChain.extent().height,
                              0.1f, 10.0f);

  ubo.proj[1][1] *= -1;

  void* data;
  vkMapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory(), 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory());
}