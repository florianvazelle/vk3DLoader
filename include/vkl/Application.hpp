/**
 * @file Application.hpp
 * @brief Define Application class
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

#include <vkl/Basic/BasicCommandBuffers.hpp>
#include <vkl/Basic/BasicDescriptorSets.hpp>
#include <vkl/Basic/BasicGraphicsPipeline.hpp>
#include <vkl/Basic/BasicRenderPass.hpp>
#include <vkl/DebugUtilsMessenger.hpp>
#include <vkl/Depth/DepthCommandBuffers.hpp>
#include <vkl/Depth/DepthDescriptorSets.hpp>
#include <vkl/Depth/DepthGraphicsPipeline.hpp>
#include <vkl/Depth/DepthRenderPass.hpp>
#include <vkl/DescriptorPool.hpp>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/DescriptorSets.hpp>
#include <vkl/Device.hpp>
#include <vkl/GraphicsPipeline.hpp>
#include <vkl/ImGui/ImGuiApp.hpp>
#include <vkl/Instance.hpp>
#include <vkl/Model.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/SyncObjects.hpp>
#include <vkl/Window.hpp>
#include <vkl/buffer/Buffer.hpp>
#include <vkl/buffer/UniformBuffers.hpp>

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

  class Application {
  public:
    Application(DebugOption debugOption, const std::string& modelPath = "");

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