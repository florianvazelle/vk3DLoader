/**
 * @file ShadowMapping.hpp
 * @brief Define ShadowMapping class
 *
 * This is our Vulkan App, call by the main and run the main loop.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <common/DebugUtilsMessenger.hpp>
#include <common/DescriptorPool.hpp>
#include <common/DescriptorSetLayout.hpp>
#include <common/DescriptorSets.hpp>
#include <common/Device.hpp>
#include <common/GraphicsPipeline.hpp>
#include <common/ImGui/ImGuiApp.hpp>
#include <common/Instance.hpp>
#include <common/Model.hpp>
#include <common/SwapChain.hpp>
#include <common/SyncObjects.hpp>
#include <common/Window.hpp>
#include <common/buffer/Buffer.hpp>
#include <common/buffer/UniformBuffers.hpp>
#include <shadow/Basic/BasicCommandBuffers.hpp>
#include <shadow/Basic/BasicDescriptorSets.hpp>
#include <shadow/Basic/BasicGraphicsPipeline.hpp>
#include <shadow/Basic/BasicRenderPass.hpp>
#include <shadow/Depth/DepthCommandBuffers.hpp>
#include <shadow/Depth/DepthDescriptorSets.hpp>
#include <shadow/Depth/DepthGraphicsPipeline.hpp>
#include <shadow/Depth/DepthRenderPass.hpp>

/**
 * @brief Vulkan Loader
 *
 * Encapsulates the entire vk3DLoader library.
 */
namespace vkl {

  struct DebugOption {
    int debugLevel;
    bool exitOnError;
  };

  class ShadowMapping {
  public:
    ShadowMapping(const std::string& appName, DebugOption debugOption, const std::string& modelPath = "");

    void run() { mainLoop(); }

  private:
    // Note : Order is taken into account

    Model model;

    Instance instance;
    DebugUtilsMessenger debugMessenger;
    Window window;
    Device device;

    SwapChain swapChain;
    CommandPool commandPool;

    VertexBuffer vertexBuffer;
    MVPUniformBuffers uniformBuffers;
    DepthUniformBuffers depthUniformBuffer;
    MaterialBuffer materialUniformBuffer;

    /**
     * Depth
     */
    DepthRenderPass rpDepth;
    DescriptorSetLayout dslDepth;
    DepthGraphicsPipeline gpDepth;

    const std::vector<VkDescriptorPoolSize> psDepth;
    VkDescriptorPoolCreateInfo dpiDepth;
    DescriptorPool dpDepth;
    DepthDescriptorSets dsDepth;
    DepthCommandBuffers cbDepth;

    /**
     * Basic
     */
    BasicRenderPass rpBasic;
    DescriptorSetLayout dslBasic;
    BasicGraphicsPipeline gpBasic;

    const std::vector<VkDescriptorPoolSize> psBasic;
    VkDescriptorPoolCreateInfo dpiBasic;
    DescriptorPool dpBasic;
    BasicDescriptorSets dsBasic;
    BasicCommandBuffers cbBasic;

    SyncObjects syncObjects;

    ImGuiApp interface;

    size_t currentFrame = 0;

    void mainLoop();

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized);
  };

}  // namespace vkl