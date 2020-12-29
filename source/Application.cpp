#include <vkl/Application.hpp>

using namespace vkl;

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<Vertex> triangle
    = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

Application::Application()
    : instance(APP_NAME, ENGINE_NAME, true),
      debugMessenger(instance),
      window({WIDTH, HEIGHT}, APP_NAME, instance),
      device(instance, window, Instance::DeviceExtensions),
      vertexTriangleBuffer(device, triangle),
      swapChain(device, window),
      renderPass(device, swapChain),
      graphicsPipeline(device, swapChain, renderPass),
      commandPool(device, 0),
      commandBuffers(device, renderPass, swapChain, graphicsPipeline, commandPool, vertexTriangleBuffer),
      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT),
      /* ImGui */ interface(instance, window, device, swapChain, graphicsPipeline) {}

void Application::mainLoop() {
  window.setDrawFrameFunc([this](bool& framebufferResized) {
    drawImGui();
    drawFrame(framebufferResized);
  });

  window.mainLoop();
  vkDeviceWaitIdle(device.logical());
}

/**
 * Note Exposé :
 * La fonction réalisera les opérations suivantes :
 * - Acquérir une image depuis la swap chain
 * - Exécuter le command buffer correspondant au framebuffer dont l'attachement est l'image obtenue
 * - Retourner l'image à la swap chain pour présentation
 *
 * Les opérations sont par défaut exécutées de manière asynchrones, or chacune des opérations que
 * nous voulons lancer dépendent des résultats de l'opération la précédant
 *
 * Voir SyncObject.
 *
 */

void Application::drawFrame(bool& framebufferResized) {
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

  // Record UI draw data
  interface.recordCommandBuffers(imageIndex);

  VkSemaphore waitSemaphores[]      = {syncObjects.imageAvailable(currentFrame)};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkCommandBuffer cmdBuffers[]   = {commandBuffers.command(imageIndex), interface.command(imageIndex)};
  VkSemaphore signalSemaphores[] = {syncObjects.renderFinished(currentFrame)};

  VkSubmitInfo submitInfo = {
      .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount   = 1,
      .pWaitSemaphores      = waitSemaphores,
      .pWaitDstStageMask    = waitStages,
      .commandBufferCount   = 2,
      .pCommandBuffers      = cmdBuffers,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores    = signalSemaphores,
  };

  vkResetFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame));

  if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, syncObjects.inFlightFence(currentFrame)) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkSwapchainKHR swapChains[] = {swapChain.handle()};

  VkPresentInfoKHR presentInfo = {
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

void Application::drawImGui() {
  // Start the Dear ImGui frame
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static float f     = 0.0f;
  static int counter = 0;

  ImGui::Begin("Renderer Options");
  ImGui::Text("This is some useful text.");
  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  if (ImGui::Button("Button")) {
    counter++;
  }
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);
  ImGui::End();

  ImGui::Render();
}

// for resize window
void Application::recreateSwapChain(bool& framebufferResized) {
  framebufferResized = true;

  glm::ivec2 size;
  window.framebufferSize(size);
  while (size[0] == 0 || size[1] == 0) {
    window.framebufferSize(size);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device.logical());

  swapChain.recreate();
  renderPass.recreate();
  graphicsPipeline.recreate();
  commandBuffers.recreate();

  interface.recreate();

  renderPass.cleanupOld();
  swapChain.cleanupOld();
}