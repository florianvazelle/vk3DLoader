// clang-format off
#include <shadow/ShadowMapping.hpp>
#include <chrono>                                  // for duration, operator-
#include <common/misc/DescriptorPool.hpp>          // for descriptorPoolSize
#include <common/misc/DescriptorSetLayout.hpp>     // for descriptorSetLayou...
#include <cstdint>                                 // for uint32_t, UINT64_MAX
#include <cstring>                                 // for memcpy
#include <deque>                                   // for deque
#include <memory>                                  // for allocator_traits<>...
#include <stdexcept>                               // for runtime_error
#include <GLFW/glfw3.h>                            // for glfwWaitEvents
#include <common/Application.hpp>                  // for Application, Debug...
#include <common/DebugUtilsMessenger.hpp>          // for vkl
#include <common/DescriptorPool.hpp>               // for DescriptorPool
#include <common/Device.hpp>                       // for Device
#include <common/ImGui/ImGuiApp.hpp>               // for ImGuiApp
#include <common/Model.hpp>                        // for Model
#include <common/SwapChain.hpp>                    // for SwapChain
#include <common/SyncObjects.hpp>                  // for SyncObjects
#include <common/Window.hpp>                       // for Window
#include <common/buffer/Buffer.hpp>                // for Buffer
#include <common/buffer/UniformBuffers.hpp>        // for UniformBuffers
#include <common/struct/Depth.hpp>                 // for Depth
#include <common/struct/DepthMVP.hpp>              // for DepthMVP
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>                                 // for Text, ColorEdit3
#include <imgui_impl_glfw.h>                       // for ImGui_ImplGlfw_New...
#include <imgui_impl_vulkan.h>                     // for ImGui_ImplVulkan_N...
#include <shadow/Basic/BasicCommandBuffers.hpp>    // for BasicCommandBuffers
#include <shadow/Basic/BasicDescriptorSets.hpp>    // for BasicDescriptorSets
#include <shadow/Basic/BasicGraphicsPipeline.hpp>  // for BasicGraphicsPipeline
#include <shadow/Basic/BasicRenderPass.hpp>        // for BasicRenderPass
#include <shadow/Depth/DepthCommandBuffers.hpp>    // for DepthCommandBuffers
#include <shadow/Depth/DepthDescriptorSets.hpp>    // for DepthDescriptorSets
#include <shadow/Depth/DepthGraphicsPipeline.hpp>  // for DepthGraphicsPipeline
#include <shadow/Depth/DepthRenderPass.hpp>        // for DepthRenderPass
#include <common/buffer/IBuffer.hpp>               // for IBuffer
#include <common/RenderPass.hpp>                   // for RenderPass
#include <common/buffer/IBuffer.hpp>               // for IUniformBuffers
// clang-format on

using namespace vkl;

struct Light {
  glm::vec3 position;
  glm::vec3 axis;
};

static Light light = {
    .axis = glm::vec3(1.f, 1.5f, 1.f),
};

void updateBasicUniformBuffers(const Device& device,
                               const SwapChain& swapChain,
                               std::deque<Buffer<DepthMVP>>& uniformBuffers,
                               float time,
                               uint32_t currentImage) {
  DepthMVP& ubo = uniformBuffers.at(currentImage).data().at(0);

  ubo.model = glm::mat4(1.0f);
  ubo.view  = glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  const float aspect = swapChain.extent().width / (float)swapChain.extent().height;
  ubo.proj           = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

  // ubo.model = glm::mat4(1.0f);
  // ubo.view  = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0, 1, 0));
  // ubo.proj  = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 100.0f);

  ubo.proj[1][1] *= -1;

  light.position = glm::vec3(glm::vec4(light.axis, 1)
                             * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

  ubo.lightPos = light.position;

  void* data;
  vkMapMemory(device.logical(), uniformBuffers[currentImage].memory(), 0, sizeof(DepthMVP), 0, &data);
  memcpy(data, &ubo, sizeof(DepthMVP));
  vkUnmapMemory(device.logical(), uniformBuffers[currentImage].memory());
}

void updateDepthUniformBuffers(const Device& device,
                               const SwapChain& swapChain,
                               std::deque<Buffer<Depth>>& uniformBuffers,
                               float time,
                               uint32_t currentImage) {
  Depth& ubo = uniformBuffers.at(currentImage).data().at(0);

  // Keep depth range as small as possible
  // for better shadow map precision
  float zNear = 0.1f;
  float zFar  = 10.0f;

  float lightFOV = 45.0f;

  // Matrix from light's point of view
  glm::mat4 depthModel = glm::mat4(1.0f);
  glm::mat4 depthView  = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0, 1, 0));
  glm::mat4 depthProj  = glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar);

  depthProj[1][1] *= -1;

  ubo.depthMVP = depthProj * depthView * depthModel;

  void* data;
  vkMapMemory(device.logical(), uniformBuffers[currentImage].memory(), 0, sizeof(Depth), 0, &data);
  memcpy(data, &ubo, sizeof(Depth));
  vkUnmapMemory(device.logical(), uniformBuffers[currentImage].memory());
}

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

ShadowMapping::ShadowMapping(const std::string& appName, const DebugOption& debugOption, const std::string& modelPath)
    : Application(appName, debugOption),

      model(modelPath),
      commandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),

      // Buffer
      vertexBuffer(device,
                   model.vertices(),
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
      uniformBuffers(device, swapChain, &updateBasicUniformBuffers),
      materialUniformBuffer(device,
                            model.materials(),
                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
      depthUniformBuffer(device, swapChain, &updateDepthUniformBuffers),

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
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain.numImages()),
      }),
      dpiDepth(misc::descriptorPoolCreateInfo(psDepth, swapChain.numImages())),
      dpDepth(device, dpiDepth),

      // ~ My Vectors
      // Utile car sinon les pointeurs change, donc on copie d'abord par valeur
      // et on passe le vecteur qui sera concervé dans la class Application
      vecRPDepth({&rpDepth}),
      vecUBDepth({&depthUniformBuffer}),
      vecVertexBuffer({&vertexBuffer}),

      // 5. Descriptor Sets
      dsDepth(device, swapChain, dslDepth, dpDepth, vecRPDepth, {}, vecUBDepth),

      // 6. Command Buffers
      cbDepth(device, rpDepth, swapChain, gpDepth, commandPool, dsDepth, vecVertexBuffer),

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
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain.numImages() * 2),
          misc::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapChain.numImages()),
      }),
      dpiBasic(misc::descriptorPoolCreateInfo(psBasic, swapChain.numImages())),
      dpBasic(device, dpiBasic),

      // ~ My Vectors 2
      vecRPBasic({&rpDepth}),
      vecUBBasic({&uniformBuffers}),
      vecBBasic({&materialUniformBuffer}),

      // 5. Descriptor Sets
      dsBasic(device, swapChain, dslBasic, dpBasic, vecRPBasic, vecBBasic, vecUBBasic),

      // 6. Command Buffers
      cbBasic(device, rpBasic, swapChain, gpBasic, commandPool, dsBasic, vecVertexBuffer),

      /* ImGui */
      interface(instance, window, device, swapChain, gpBasic) {}

void ShadowMapping::mainLoop() {
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

void ShadowMapping::drawFrame(bool& framebufferResized) {
  uint32_t imageIndex;
  VkResult result = prepareFrame(framebufferResized, imageIndex);
  if (result != VK_SUCCESS) return;

  // Record UI draw data
  interface.recordCommandBuffers(imageIndex);
  cbDepth.recordCommandBuffers(imageIndex);

  /* Update Uniform Buffers */

  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  depthUniformBuffer.update(time, imageIndex);
  uniformBuffers.data(imageIndex).at(0).depthBiasMVP = depthUniformBuffer.data(imageIndex).at(0).depthMVP;
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

  submitFrame(framebufferResized, imageIndex);

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void ShadowMapping::drawImGui() {
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
    ImGui::Separator();
    ImGui::Text("Light Setting");
    ImGui::SliderFloat3("Axis", glm::value_ptr(light.axis), 1.0f, 5.0f);
    ImGui::Separator();
    ImGui::Text("Material Setting");
    ImGui::ColorEdit3("diffuse", glm::value_ptr(materialUniformBuffer.data().at(0).diffuse));
    ImGui::ColorEdit3("specular", glm::value_ptr(materialUniformBuffer.data().at(0).specular));
    ImGui::ColorEdit3("ambient", glm::value_ptr(materialUniformBuffer.data().at(0).ambient));
    ImGui::SliderFloat("shininess", &(materialUniformBuffer.data().at(0).shininess), 0.5f, 256.0f);
    ImGui::Separator();
    ImGui::Text("Depth Setting");
    ImGui::SliderFloat("constant", &cbDepth.depthBiasConstant(), 0.0f, 5.0f);
    ImGui::SliderFloat("slope", &cbDepth.depthBiasSlope(), 0.0f, 5.0f);
  }

  ImGui::End();
  ImGui::Render();
}

// for resize window
void ShadowMapping::recreateSwapChain(bool& framebufferResized) {
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
