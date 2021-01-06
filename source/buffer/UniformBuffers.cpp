#include <vkl/buffer/UniformBuffers.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstring>
#include <vkl/struct/Depth.hpp>
#include <vkl/struct/MVP.hpp>

using namespace vkl;

static glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);

template <> void UniformBuffers<vkl::MVP>::update(float time, uint32_t currentImage) {
  m_ubo.model = glm::mat4(1.0f);
  m_ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  const float aspect = m_swapChain.extent().width / (float)m_swapChain.extent().height;
  m_ubo.proj         = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);

  // m_ubo.model = glm::mat4(1.0f);
  // m_ubo.view  = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
  // m_ubo.proj  = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);

  m_ubo.proj[1][1] *= -1;

  lightPos
      = glm::vec3(glm::vec4(1) * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

  m_ubo.lightPos = lightPos;

  void* data;
  vkMapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory(), 0, sizeof(m_ubo), 0, &data);
  memcpy(data, &m_ubo, sizeof(m_ubo));
  vkUnmapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory());
}

template <> void UniformBuffers<vkl::Depth>::update(float time, uint32_t currentImage) {
  // Keep depth range as small as possible
  // for better shadow map precision
  float zNear = 0.1f;
  float zFar  = 100.0f;

  float lightFOV = 45.0f;

  // Matrix from light's point of view
  glm::mat4 depthModel = glm::mat4(1.0f);
  glm::mat4 depthView  = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
  glm::mat4 depthProj  = glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar);

  depthProj[1][1] *= -1;

  m_ubo.depthMVP = depthProj * depthView * depthModel;

  void* data;
  vkMapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory(), 0, sizeof(m_ubo), 0, &data);
  memcpy(data, &m_ubo, sizeof(m_ubo));
  vkUnmapMemory(m_device.logical(), m_uniformBuffers[currentImage].memory());
}