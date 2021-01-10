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
#include <vkl/Basic/BasicRenderPass.hpp>
#include <vkl/DebugUtilsMessenger.hpp>
#include <vkl/Depth/DepthCommandBuffers.hpp>
#include <vkl/Depth/DepthRenderPass.hpp>
#include <vkl/DescriptorPool.hpp>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/DescriptorSets.hpp>
#include <vkl/Device.hpp>
#include <vkl/GraphicsPipeline.hpp>
#include <vkl/ImGui/ImGuiApp.hpp>
#include <vkl/Instance.hpp>
#include <vkl/Model.hpp>
#include <vkl/ShaderLoader.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/SyncObjects.hpp>
#include <vkl/Window.hpp>
#include <vkl/buffer/Buffer.hpp>
#include <vkl/buffer/UniformBuffers.hpp>

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

    VertexBuffer vertexBuffer;
    DescriptorSetLayout descriptorSetLayout;

    SwapChain swapChain;
    MVPUniformBuffers uniformBuffers;
    DepthUniformBuffers depthUniformBuffer;
    MaterialBuffer materialUniformBuffer;

    BasicRenderPass renderPass;
    DepthRenderPass depthRenderPass;

    const std::vector<VkDescriptorPoolSize> poolSizes;
    VkDescriptorPoolCreateInfo descPoolInfo;
    DescriptorPool descriptorPool;
    DescriptorSets descriptorSets;

    GraphicsPipeline graphicsPipeline;
    CommandPool commandPool;

    BasicCommandBuffers commandBuffers;
    DepthCommandBuffers depthCommandBuffers;

    SyncObjects syncObjects;

    ImGuiApp interface;

    size_t currentFrame = 0;

    void mainLoop();

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized);
  };

}  // namespace vkl