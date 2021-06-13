// clang-format off
#include <particle/ParticleSystem.hpp>
#include <chrono>                                        // for duration
#include <common/misc/DescriptorPool.hpp>                // for descriptorPo...
#include <common/misc/DescriptorSetLayout.hpp>           // for descriptorSe...
#include <cstdint>                                       // for uint32_t
#include <cstring>                                       // for memcpy
#include <deque>                                         // for deque
#include <memory>                                        // for allocator_tr...
#include <stdexcept>                                     // for runtime_error
#include <GLFW/glfw3.h>                                  // for glfwWaitEvents
#include <common/Application.hpp>                        // for Application
#include <common/DebugUtilsMessenger.hpp>                // for vkl
#include <common/Device.hpp>                             // for Device
#include <common/Semaphore.hpp>                          // for Semaphore
#include <common/SwapChain.hpp>                          // for SwapChain
#include <common/SyncObjects.hpp>                        // for SyncObjects
#include <common/Window.hpp>                             // for Window
#include <common/buffer/Buffer.hpp>                      // for Buffer
#include <common/buffer/UniformBuffers.hpp>              // for UniformBuffers
#include <common/struct/ComputeParticle.hpp>             // for ComputeParticle
#include <common/struct/MVP.hpp>                         // for MVP
#include <common/struct/Particle.hpp>                    // for Particle
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>                                       // for Text, Begin
#include <imgui_impl_glfw.h>                             // for ImGui_ImplGl...
#include <imgui_impl_vulkan.h>                           // for ImGui_ImplVu...
#include <particle/Compute/ComputeCommandBuffer.hpp>     // for ComputeComma...
#include <particle/Compute/ComputeDescriptorSets.hpp>    // for ComputeDescr...
#include <particle/Graphic/GraphicDescriptorSets.hpp>    // for GraphicDescr...
#include <particle/Graphic/GraphicGraphicsPipeline.hpp>  // for GraphicGraph...
#include <shadow/Basic/BasicRenderPass.hpp>              // for BasicRenderPass
#include <common/buffer/IBuffer.hpp>                     // for IBuffer
// clang-format on

using namespace vkl;

const std::vector<Particle> particles(NUM_PARTICLE, {{1, 1, 1, 1}, {1, 1, 1, 1}});

void updateGraphicsUniformBuffers(const Device& device,
                                  const SwapChain& swapChain,
                                  std::deque<Buffer<MVP>>& uniformBuffers,
                                  float time,
                                  uint32_t currentImage) {
  MVP& ubo = uniformBuffers.at(currentImage).data().at(0);

  ubo.model = glm::mat4(1.0f);
  ubo.view  = glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  const float aspect = swapChain.extent().width / (float)swapChain.extent().height;
  ubo.proj           = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
  ubo.proj[1][1] *= -1;

  void* data;
  vkMapMemory(device.logical(), uniformBuffers[currentImage].memory(), 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(device.logical(), uniformBuffers[currentImage].memory());
}

void updateComputeUniformBuffers(const Device& device,
                                 const SwapChain& swapChain,
                                 std::deque<Buffer<ComputeParticle>>& uniformBuffers,
                                 float time,
                                 uint32_t currentImage) {
  ComputeParticle& ubo = uniformBuffers.at(currentImage).data().at(0);

  ubo.deltaT = time * 0.05f;

  void* data;
  vkMapMemory(device.logical(), uniformBuffers[currentImage].memory(), 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(device.logical(), uniformBuffers[currentImage].memory());
}

ParticleSystem::ParticleSystem(const std::string& appName, const DebugOption& debugOption)
    : Application(appName, debugOption),

      commandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
      // Use a separate command pool (queue family may differ from the one used for graphics)
      commandPoolCompute(device,
                         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                         device.queueFamilyIndices().computeFamily),

      // Descriptor Pool
      ps({
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain.numImages() * 2),
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, swapChain.numImages()),
      }),
      dpi(misc::descriptorPoolCreateInfo(
          ps,
          swapChain.numImages() * 2)),  // * 2 car on utilise le meme descriptor pool pour les Graphic et Compute
      dp(device, dpi),

      // Buffer
      // Graphic
      particleBuffer(device,
                     particles,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
      uniformBuffersGraphic(device, swapChain, &updateGraphicsUniformBuffers),

      // Compute
      storageBuffer(
          device,
          NUM_PARTICLE * sizeof(Particle),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      uniformBuffersCompute(device, swapChain, &updateComputeUniformBuffers),

      // ~ My Vectors
      // Utile car sinon les pointeurs change, donc on copie d'abord par valeur
      // et on passe le vecteur qui sera concervé dans la class Application
      vecRPGraphic({&rpGraphic}),
      vecUBGraphic({&uniformBuffersGraphic}),
      vecUBCompute({&uniformBuffersCompute}),
      vecSBCompute({&storageBuffer}),

      /*
       * Basic Graphics
       */

      // 1. Render Pass
      rpGraphic(device, swapChain),

      // 2. Descriptor Set Layout
      dslGraphic(device,
                 misc::descriptorSetLayoutCreateInfo({
                     misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 2),
                 })),

      // 3. Graphic Pipeline
      gpGraphic(device, swapChain, rpGraphic, dslGraphic),

      // 5. Descriptor Sets
      dsGraphic(device, swapChain, dslGraphic, dp, vecRPGraphic, {}, vecUBGraphic),

      cbGraphic(device, rpGraphic, swapChain, gpGraphic, commandPool, dsGraphic, vecSBCompute),

      // Semaphore for compute & graphics sync
      semaphoreGraphic(device),

      /*
       * Compute
       */

      // 2. Descriptor Set Layout
      dslCompute(
          device,
          misc::descriptorSetLayoutCreateInfo({
              // Binding 0 : Particle position storage buffer
              misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0),
              // Binding 1 : Uniform buffer
              misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1),
          })),

      // 5. Descriptor Sets
      dsCompute(device, swapChain, dslCompute, dp, vecRPGraphic, vecSBCompute, vecUBCompute),

      // 3. Compute Pipeline
      gpCompute(device, swapChain, rpGraphic, dslCompute),

      semaphoreCompute(device),

      // signal semaphore

      cbCompute(device,
                rpGraphic,
                swapChain,
                gpCompute,
                storageBuffer,
                commandPoolCompute,
                semaphoreCompute,
                dsCompute),

      /* ImGui */
      interface(instance, window, device, swapChain, gpGraphic) {}

void ParticleSystem::run() {
  window.setDrawFrameFunc([this](bool& framebufferResized) {
    drawImGui();
    drawFrame(framebufferResized);
  });

  window.mainLoop();
  vkDeviceWaitIdle(device.logical());
}

void ParticleSystem::drawFrame(bool& framebufferResized) {
  uint32_t imageIndex;
  VkResult result = prepareFrame(framebufferResized, imageIndex);
  if (result != VK_SUCCESS) return;

  /* Update Uniform Buffers */

  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  uniformBuffersGraphic.update(time, imageIndex);
  uniformBuffersCompute.update(time, imageIndex);

  /* Submit */
  const VkPipelineStageFlags graphicsWaitStageMasks[]
      = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  const VkSemaphore graphicsWaitSemaphores[]   = {semaphoreCompute.handle(), syncObjects.imageAvailable(currentFrame)};
  const VkSemaphore graphicsSignalSemaphores[] = {semaphoreGraphic.handle(), syncObjects.renderFinished(currentFrame)};

  std::vector<VkCommandBuffer> cmdBuffers = {
      cbGraphic.command(imageIndex),
      interface.command(imageIndex),
  };

  // Submit graphics commands
  {
    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 2,
        .pWaitSemaphores      = graphicsWaitSemaphores,
        .pWaitDstStageMask    = graphicsWaitStageMasks,
        .commandBufferCount   = static_cast<uint32_t>(cmdBuffers.size()),
        .pCommandBuffers      = cmdBuffers.data(),
        .signalSemaphoreCount = 2,
        .pSignalSemaphores    = graphicsSignalSemaphores,
    };

    vkResetFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame));

    if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, syncObjects.inFlightFence(currentFrame)) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }
  }

  // Submit frame
  submitFrame(framebufferResized, imageIndex);

  // Wait for rendering finished
  VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

  // Submit compute commands
  {
    const VkSubmitInfo computeSubmitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &semaphoreGraphic.handle(),
        .pWaitDstStageMask    = &waitStageMask,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cbCompute.command(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &semaphoreCompute.handle(),
    };

    if (vkQueueSubmit(device.computeQueue(), 1, &computeSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void ParticleSystem::drawImGui() {
  // Start the Dear ImGui frame
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  // Display ImGui components
  ImGui::Begin("Config");

  {
    static int frame = 0;
    ImGui::Text("frame: %d", ++frame);
    ImGui::Text("time: %.2f", time);
    ImGui::Text("fps: %.2f", ImGui::GetIO().Framerate);
  }

  ImGui::End();
  ImGui::Render();
}

// for resize window
void ParticleSystem::recreateSwapChain(bool& framebufferResized) {
  glm::ivec2 size;
  window.framebufferSize(size);
  while (size[0] == 0 || size[1] == 0) {
    window.framebufferSize(size);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device.logical());

  swapChain.recreate();

  // Recreated because the number of buffer is based on number of image in swapchain
  uniformBuffersGraphic.recreate();
  uniformBuffersCompute.recreate();

  /**
   * Graphic
   */
  rpGraphic.recreate();
  gpGraphic.recreate();
  dp.recreate();
  dsGraphic.recreate();
  cbGraphic.recreate();

  /**
   * Compute
   */
  gpCompute.recreate();
  dsCompute.recreate();
  cbCompute.recreate();

  interface.recreate();

  swapChain.cleanupOld();
}
