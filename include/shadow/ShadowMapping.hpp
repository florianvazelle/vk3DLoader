/**
 * @file ShadowMapping.hpp
 * @brief Define ShadowMapping class
 *
 * This is our Vulkan App, call by the main and run the main loop.
 */

#pragma once

// clang-format off
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <common/VulkanHeader.hpp>                    // for VkDescriptorPoolSize
#include <common/Application.hpp>                  // for Application
#include <common/DescriptorPool.hpp>               // for DescriptorPool
#include <common/DescriptorSetLayout.hpp>          // for DescriptorSetLayout
#include <common/ImGui/ImGuiApp.hpp>               // for ImGuiApp
#include <common/Model.hpp>                        // for Model
#include <common/buffer/Buffer.hpp>                // for Buffer
#include <common/buffer/UniformBuffers.hpp>        // for UniformBuffers
#include <common/struct/Depth.hpp>                 // for Depth
#include <common/struct/DepthMVP.hpp>              // for DepthMVP
#include <cstdlib>                                 // for size_t
#include <shadow/Basic/BasicCommandBuffers.hpp>    // for BasicCommandBuffers
#include <shadow/Basic/BasicDescriptorSets.hpp>    // for BasicDescriptorSets
#include <shadow/Basic/BasicGraphicsPipeline.hpp>  // for BasicGraphicsPipeline
#include <shadow/Basic/BasicRenderPass.hpp>        // for BasicRenderPass
#include <shadow/Depth/DepthCommandBuffers.hpp>    // for DepthCommandBuffers
#include <shadow/Depth/DepthDescriptorSets.hpp>    // for DepthDescriptorSets
#include <shadow/Depth/DepthGraphicsPipeline.hpp>  // for DepthGraphicsPipeline
#include <shadow/Depth/DepthRenderPass.hpp>        // for DepthRenderPass
#include <string>                                  // for string
#include <vector>                                  // for allocator, vector
#include <common/CommandPool.hpp>                  // for CommandPool
#include <common/struct/Material.hpp>              // for Material
#include <common/struct/Vertex.hpp>                // for Vertex
#include <common/image/Texture.hpp>                      // for Texture
// clang-format on

namespace vkl {

  class ShadowMapping : public Application {
  public:
    ShadowMapping(const std::string& appName, const DebugOption& debugOption, const std::string& modelPath = "");

    void run() { mainLoop(); }

  private:
    // Note : Order is taken into account

    CommandPool commandPool;

    Model model;

    Buffer<Vertex> vertexBuffer;
    UniformBuffers<DepthMVP> uniformBuffers;
    Buffer<Material> materialUniformBuffer;
    UniformBuffers<Depth> depthUniformBuffer;

    /**
     * Depth
     */
    DepthRenderPass rpDepth;
    DescriptorSetLayout dslDepth;
    DepthGraphicsPipeline gpDepth;

    const std::vector<VkDescriptorPoolSize> psDepth;
    VkDescriptorPoolCreateInfo dpiDepth;
    DescriptorPool dpDepth;

    std::vector<const IUniformBuffers*> vecUBDepth;
    std::vector<const IBuffer*> vecVertexBuffer;

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

    std::vector<const Image*> vecIBasic;
    std::vector<const IUniformBuffers*> vecUBBasic;
    std::vector<const IBuffer*> vecBBasic;

    BasicDescriptorSets dsBasic;
    BasicCommandBuffers cbBasic;

    ImGuiApp interface;

    void mainLoop();

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized) final;
  };

}  // namespace vkl
