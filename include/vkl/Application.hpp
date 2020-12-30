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
#include <vkl/DescriptorPool.hpp>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/DescriptorSets.hpp>
#include <vkl/Device.hpp>
#include <vkl/GraphicsPipeline.hpp>
#include <vkl/ImGui/ImGuiApp.hpp>
#include <vkl/Instance.hpp>
#include <vkl/ShaderLoader.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/SyncObjects.hpp>
#include <vkl/Window.hpp>
#include <vkl/buffer/UniformBuffers.hpp>
#include <vkl/buffer/VertexBuffer.hpp>

namespace vkl {

  class Application {
  public:
    Application();

    void run() { mainLoop(); }

  private:
    // Note : Order is taken into account

    Instance instance;
    DebugUtilsMessenger debugMessenger;
    Window window;
    Device device;

    VertexBuffer vertexTriangleBuffer;
    DescriptorSetLayout descriptorSetLayout;

    SwapChain swapChain;
    UniformBuffers uniformBuffers;
    DescriptorPool descriptorPool;
    DescriptorSets descriptorSets;

    BasicRenderPass renderPass;
    GraphicsPipeline graphicsPipeline;
    CommandPool commandPool;
    BasicCommandBuffers commandBuffers;
    SyncObjects syncObjects;

    ImGuiApp interface;

    size_t currentFrame = 0;

    void mainLoop();

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized);
  };

}  // namespace vkl