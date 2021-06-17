// clang-format off
#include <common/Application.hpp>
#include <common/DebugUtilsMessenger.hpp>  // for vkl
#include <common/Instance.hpp>             // for Instance, Instance::Device...
#include <common/SwapChain.hpp>            // for SwapChain
#include <glm/glm.hpp>
// clang-format on

using namespace vkl;

Application::Application(const std::string& appName, const DebugOption& debugOption)
    : instance(appName, ENGINE_NAME, (debugOption.debugLevel > 0)),
      debugMessenger(instance, debugOption.exitOnError),
      window({WIDTH, HEIGHT}, appName, instance),
      device(instance, window, Instance::DeviceExtensions),
      swapChain(device, window),
      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT) {}

VkResult Application::prepareFrame(bool& framebufferResized, uint32_t& imageIndex) {
  // vkWaitForFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame), VK_TRUE, UINT64_MAX);

  // Get image from swap chain
  VkResult result = vkAcquireNextImageKHR(device.logical(), swapChain.handle(), UINT64_MAX,
                                          syncObjects.imageAvailable(currentFrame), VK_NULL_HANDLE, &imageIndex);
  // Create new swap chain if needed
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain(framebufferResized);
    // return VK_ERROR_OUT_OF_DATE_KHR;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swapchain image");
  }

  // if (syncObjects.imageInFlight(imageIndex) != VK_NULL_HANDLE) {
  //   vkWaitForFences(device.logical(), 1, &syncObjects.imageInFlight(imageIndex), VK_TRUE, UINT64_MAX);
  // }
  // syncObjects.imageInFlight(imageIndex) = syncObjects.inFlightFence(currentFrame);

  return VK_SUCCESS;
}

void Application::submitFrame(bool& framebufferResized, const uint32_t& imageIndex) {
  const VkPresentInfoKHR presentInfo = {
      .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores    = &(syncObjects.renderFinished(currentFrame)),
      .swapchainCount     = 1,
      .pSwapchains        = &swapChain.handle(),
      .pImageIndices      = &imageIndex,
  };

  VkResult result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    recreateSwapChain(framebufferResized);
    framebufferResized = false;
    return;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image");
  }

  vkQueueWaitIdle(device.presentQueue());
}