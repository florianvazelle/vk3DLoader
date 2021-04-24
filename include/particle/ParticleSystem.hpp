// /**
//  * @file ParticleSystem.hpp
//  * @brief Define ParticleSystem class
//  *
//  * This is our Vulkan App, call by the main and run the main loop.
//  */

// #pragma once

// #include <algorithm>
// #include <array>
// #include <cstdint>
// #include <cstdlib>
// #include <cstring>
// #include <fstream>
// #include <iostream>
// #include <optional>
// #include <set>
// #include <stdexcept>
// #include <vector>

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

// #include <imgui.h>
// #include <imgui_impl_glfw.h>
// #include <imgui_impl_vulkan.h>

// #include <common/DebugUtilsMessenger.hpp>
// #include <common/DescriptorPool.hpp>
// #include <common/DescriptorSetLayout.hpp>
// #include <common/DescriptorSets.hpp>
// #include <common/Device.hpp>
// #include <common/GraphicsPipeline.hpp>
// #include <common/ImGui/ImGuiApp.hpp>
// #include <common/Instance.hpp>
// #include <common/Model.hpp>
// #include <common/SwapChain.hpp>
// #include <common/SyncObjects.hpp>
// #include <common/Window.hpp>
// #include <common/buffer/Buffer.hpp>
// #include <common/buffer/UniformBuffers.hpp>
// #include <shadow/Basic/BasicCommandBuffers.hpp>
// #include <shadow/Basic/BasicDescriptorSets.hpp>
// #include <shadow/Basic/BasicGraphicsPipeline.hpp>
// #include <shadow/Basic/BasicRenderPass.hpp>
// #include <shadow/Depth/DepthCommandBuffers.hpp>
// #include <shadow/Depth/DepthDescriptorSets.hpp>
// #include <shadow/Depth/DepthGraphicsPipeline.hpp>
// #include <shadow/Depth/DepthRenderPass.hpp>

// namespace vkl {

//   class ParticleSystem {
//   public:
//     ParticleSystem(DebugOption debugOption);

//     void run(std::function<void(void)>& update);

//   private:
//     // Note : Order is taken into account

//     Instance instance;
//     DebugUtilsMessenger debugMessenger;
//     Window window;
//     Device device;

//     SwapChain swapChain;
//     CommandPool commandPool;

//     MVPUniformBuffers uniformBuffers;

//     /**
//      * Basic
//      */
//     BasicRenderPass rpBasic;
//     DescriptorSetLayout dslBasic;
//     BasicGraphicsPipeline gpBasic;

//     const std::vector<VkDescriptorPoolSize> psBasic;
//     VkDescriptorPoolCreateInfo dpiBasic;
//     DescriptorPool dpBasic;
//     BasicDescriptorSets dsBasic;
//     BasicCommandBuffers cbBasic;

//     SyncObjects syncObjects;

//     ImGuiApp interface;

//     size_t currentFrame = 0;

//     void drawFrame(bool& framebufferResized);
//     void drawImGui();

//     void recreateSwapChain(bool& framebufferResized);
//   };

// }  // namespace vkl