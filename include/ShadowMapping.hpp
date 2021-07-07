/**
 * @file ShadowMapping.hpp
 * @brief Define ShadowMapping class
 *
 * This is our Vulkan App, call by the main and run the main loop.
 */

#pragma once

#include <poike/poike.hpp>
#include <Model.hpp>                     
#include <struct/Depth.hpp>              
#include <struct/DepthMVP.hpp>              
#include <cstdlib>                                 
#include <Basic/BasicCommandBuffers.hpp>   
#include <Basic/BasicDescriptorSets.hpp>   
#include <Basic/BasicGraphicsPipeline.hpp>  
#include <Basic/BasicRenderPass.hpp>       
#include <Depth/DepthCommandBuffers.hpp>   
#include <Depth/DepthDescriptorSets.hpp>  
#include <Depth/DepthGraphicsPipeline.hpp>  
#include <Depth/DepthRenderPass.hpp>       
#include <string>                                  
#include <vector>                               
#include <struct/Material.hpp>            
#include <struct/Vertex.hpp>               

using namespace poike;

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
