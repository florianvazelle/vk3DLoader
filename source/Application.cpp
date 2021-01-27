#include <vkl/Application.hpp>

#include <vkl/misc/DescriptorPool.hpp>
#include <vkl/misc/DescriptorSetLayout.hpp>

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace vkl;

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

bool isRotate = true;

const int MAX_FRAMES_IN_FLIGHT = 2;

bool enabledShadowMap = true;

/**
 * Basic is for SwapChain render (default)
 * Depth is for Depth render (shadow map)
 *
 * rp  is for RenderPass
 *
 * dsl  is for DescriptorSetLayout
 *
 * gp  is for GraphicPipeline
 *
 * ps  is for PoolSize
 * dpi is for DescriptorPoolInfo
 * dp  is for DescriptorPool
 *
 * ds  is for DescriptorSets
 *
 * cb  is for CommandBuffers
 */

Application::Application(DebugOption debugOption, const std::string& modelPath)
    : instance(APP_NAME, ENGINE_NAME, (debugOption.debugLevel > 0)),
      debugMessenger(instance, debugOption.exitOnError),
      window({WIDTH, HEIGHT}, APP_NAME, instance),
      device(instance, window, Instance::DeviceExtensions),

      // Swap Chain
      swapChain(device, window),

      commandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
      model(modelPath),

      // Buffer
      vertexBuffer(device, model.vertices()),
      uniformBuffers(device, swapChain),
      materialUniformBuffer(device, model.materials().at(0)),
      depthUniformBuffer(device, swapChain),

      /**
       * Depth
       */
      // 1. Render Pass
      rpDepth(device, swapChain),

      // 2. Descriptor Set Layout
      dslDepth(device,
               misc::descriptorSetLayoutCreateInfo({
                   // Binding 0 : Vertex shader uniform buffer
                   misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
               })),

      // 3. Graphic Pipeline
      gpDepth(device, swapChain, rpDepth, dslDepth),

      // 4. Descriptor Pool
      psDepth({
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain.numImages() + 1),
      }),
      dpiDepth(misc::descriptorPoolCreateInfo(psDepth, (swapChain.numImages() + 1))),
      dpDepth(device, dpiDepth),

      // 5. Descriptor Sets
      dsDepth(device, swapChain, rpDepth, uniformBuffers, materialUniformBuffer, depthUniformBuffer, dslDepth, dpDepth),

      // 6. Command Buffers
      cbDepth(device, rpDepth, swapChain, gpDepth, commandPool, vertexBuffer, dsDepth),

      /**
       * Basic
       */
      // 1. Render Pass
      rpBasic(device, swapChain),

      // 2. Descriptor Set Layout
      dslBasic(device,
               // ce que le shader attend en entré
               misc::descriptorSetLayoutCreateInfo({
                   // Binding 0 : Vertex shader uniform buffer
                   misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
                   // Binding 1 : Fragment shader sampler (shadow map)
                   misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                    VK_SHADER_STAGE_FRAGMENT_BIT,
                                                    1),
                   // Binding 2 : Fragment shader uniform buffer (Material)
                   misc::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 2),
               })),

      // 3. Graphic Pipeline
      gpBasic(device, swapChain, rpBasic, dslBasic),

      // 4. Descriptor Pool
      psBasic({
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain.numImages() * 2 + 1),
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapChain.numImages() + 1),
      }),
      dpiBasic(misc::descriptorPoolCreateInfo(psBasic, (swapChain.numImages() * 2 + 1) + (swapChain.numImages() + 1))),
      dpBasic(device, dpiBasic),

      // 5. Descriptor Sets
      dsBasic(device, swapChain, rpDepth, uniformBuffers, materialUniformBuffer, depthUniformBuffer, dslBasic, dpBasic),

      // 6. Command Buffers
      cbBasic(device, rpBasic, swapChain, gpBasic, commandPool, vertexBuffer, dsBasic),

      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT),
      /* ImGui */
      interface(instance, window, device, swapChain, gpBasic) {}

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
  cbDepth.recordCommandBuffers(imageIndex);

  /* Update Uniform Buffers */

  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  depthUniformBuffer.update(time, imageIndex);
  uniformBuffers.data(imageIndex).depthBiasMVP = depthUniformBuffer.data(imageIndex).depthMVP;
  uniformBuffers.update(time, imageIndex);
  materialUniformBuffer.update(time, imageIndex);

  /* Submit */

  const VkSemaphore waitSemaphores[]      = {syncObjects.imageAvailable(currentFrame)};
  const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  std::vector<VkCommandBuffer> cmdBuffers = {
      cbDepth.command(imageIndex),
      cbBasic.command(imageIndex),
      interface.command(imageIndex),
  };

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

void Application::drawImGui() {
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
    ImGui::Separator();
    ImGui::Text("Light Setting");
    ImGui::SliderFloat3("Axis", glm::value_ptr(light.axis), 1.0f, 5.0f);
    ImGui::Separator();
    ImGui::Text("Material Setting");
    ImGui::ColorEdit3("diffuse", glm::value_ptr(materialUniformBuffer.data().diffuse));
    ImGui::ColorEdit3("specular", glm::value_ptr(materialUniformBuffer.data().specular));
    ImGui::ColorEdit3("ambient", glm::value_ptr(materialUniformBuffer.data().ambient));
    ImGui::SliderFloat("shininess", &(materialUniformBuffer.data().shininess), 0.5f, 256.0f);
    ImGui::Separator();
    ImGui::Text("Depth Setting");
    ImGui::SliderFloat("constant", &cbDepth.depthBiasConstant(), 0.0f, 5.0f);
    ImGui::SliderFloat("slope", &cbDepth.depthBiasSlope(), 0.0f, 5.0f);
  }

  ImGui::End();
  ImGui::Render();
}

// for resize window
void Application::recreateSwapChain(bool& framebufferResized) {
  glm::ivec2 size;
  window.framebufferSize(size);
  while (size[0] == 0 || size[1] == 0) {
    window.framebufferSize(size);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device.logical());

  swapChain.recreate();
  uniformBuffers.recreate();

  /**
   * Depth
   */
  rpDepth.recreate();
  gpDepth.recreate();
  dpDepth.recreate();
  dsDepth.recreate();
  cbDepth.recreate();

  /**
   * Basic
   */
  rpBasic.recreate();
  gpBasic.recreate();
  dpBasic.recreate();
  dsBasic.recreate();
  cbBasic.recreate();

  interface.recreate();

  swapChain.cleanupOld();
}