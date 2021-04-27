#include <particle/ParticleSystem.hpp>

#include <common/misc/DescriptorPool.hpp>
#include <common/misc/DescriptorSetLayout.hpp>

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace vkl;

ParticleSystem::ParticleSystem(const std::string& appName, const DebugOption& debugOption)
    : Application(appName, debugOption) {}

void ParticleSystem::run(std::function<void(void)> update) {
  window.setDrawFrameFunc([this, update](bool& framebufferResized) {
    update();

    drawImGui();
    drawFrame(framebufferResized);
  });

  window.mainLoop();
  vkDeviceWaitIdle(device.logical());
}

void ParticleSystem::drawFrame(bool& framebufferResized) {
  vkWaitForFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame), VK_TRUE, UINT64_MAX);

  // Get image from swap chain
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(device.logical(), swapChain.handle(), UINT64_MAX,
                                          syncObjects.imageAvailable(currentFrame), VK_NULL_HANDLE, &imageIndex);
  // Create new swap chain if needed
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain(framebufferResized);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swapchain image");
  }

  if (syncObjects.imageInFlight(imageIndex) != VK_NULL_HANDLE) {
    vkWaitForFences(device.logical(), 1, &syncObjects.imageInFlight(imageIndex), VK_TRUE, UINT64_MAX);
  }
  syncObjects.imageInFlight(imageIndex) = syncObjects.inFlightFence(currentFrame);

  /* Submit */

  const VkSemaphore waitSemaphores[]      = {syncObjects.imageAvailable(currentFrame)};
  const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  std::vector<VkCommandBuffer> cmdBuffers = {};

  const VkSemaphore signalSemaphores[] = {syncObjects.renderFinished(currentFrame)};

  const VkSubmitInfo submitInfo = {
      .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount   = 1,
      .pWaitSemaphores      = waitSemaphores,
      .pWaitDstStageMask    = waitStages,
      .commandBufferCount   = static_cast<uint32_t>(cmdBuffers.size()),
      .pCommandBuffers      = cmdBuffers.data(),
      .signalSemaphoreCount = 1,
      .pSignalSemaphores    = signalSemaphores,
  };

  vkResetFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame));

  if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, syncObjects.inFlightFence(currentFrame)) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  const VkSwapchainKHR swapChains[] = {swapChain.handle()};

  const VkPresentInfoKHR presentInfo = {
      .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores    = signalSemaphores,
      .swapchainCount     = 1,
      .pSwapchains        = swapChains,
      .pImageIndices      = &imageIndex,
  };

  result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    framebufferResized = false;
    recreateSwapChain(framebufferResized);
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void ParticleSystem::drawImGui() {
  // Start the Dear ImGui frame
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static int frame      = 0;
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  // Display ImGui components
  ImGui::Begin("Config");

  {
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
  swapChain.cleanupOld();
}