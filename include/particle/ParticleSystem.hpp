/**
 * @file ParticleSystem.hpp
 * @brief Define ParticleSystem class
 *
 * This is our Vulkan App, call by the main and run the main loop.
 */

#pragma once

// clang-format off
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>                          // for VkDescriptor...
#include <common/Application.hpp>                        // for Application
#include <common/CommandPool.hpp>                        // for CommandPool
#include <common/DescriptorPool.hpp>                     // for DescriptorPool
#include <common/DescriptorSetLayout.hpp>                // for DescriptorSe...
#include <common/Semaphore.hpp>                          // for Semaphore
#include <common/buffer/Buffer.hpp>                      // for Buffer
#include <common/buffer/StorageBuffer.hpp>               // for StorageBuffer
#include <common/buffer/UniformBuffers.hpp>              // for UniformBuffers
#include <common/struct/ComputeParticle.hpp>             // for ComputeParticle
#include <common/struct/MVP.hpp>                         // for MVP
#include <common/struct/Particle.hpp>                    // for Particle
#include <cstdlib>                                       // for size_t
#include <functional>                                    // for function
#include <particle/Compute/ComputeCommandBuffer.hpp>     // for ComputeComma...
#include <particle/Compute/ComputeDescriptorSets.hpp>    // for ComputeDescr...
#include <particle/Compute/ComputePipeline.hpp>          // for ComputePipeline
#include <particle/Graphic/GraphicCommandBuffers.hpp>    // for GraphicComma...
#include <particle/Graphic/GraphicDescriptorSets.hpp>    // for GraphicDescr...
#include <particle/Graphic/GraphicGraphicsPipeline.hpp>  // for GraphicGraph...
#include <shadow/Basic/BasicRenderPass.hpp>              // for BasicRenderPass
#include <string>                                        // for string
#include <vector>                                        // for vector
// clang-format on

#define NUM_PARTICLE 10

namespace vkl {
  class ParticleSystem : public Application {
  public:
    ParticleSystem(const std::string& appName, const DebugOption& debugOption);

    void run(std::function<void(void)> update);

  private:
    // Separate queue for compute commands (queue family may differ from the one used for graphics)
    VkQueue queueCompute;

    CommandPool commandPool, commandPoolCompute;

    // Descriptor Pool
    const std::vector<VkDescriptorPoolSize> ps;
    VkDescriptorPoolCreateInfo dpi;
    DescriptorPool dp;

    // Graphic

    BasicRenderPass rpGraphic;
    Buffer<Particle> particleBuffer;
    UniformBuffers<MVP> uniformBuffersGraphic;

    DescriptorSetLayout dslGraphic;
    GraphicGraphicsPipeline gpGraphic;
    GraphicDescriptorSets dsGraphic;
    GraphicCommandBuffers cbGraphic;
    Semaphore semaphoreGraphic;

    // Compute

    StorageBuffer storageBuffer;
    UniformBuffers<ComputeParticle> uniformBuffersCompute;
    DescriptorSetLayout dslCompute;
    ComputeDescriptorSets dsCompute;
    ComputePipeline gpCompute;
    ComputeCommandBuffer cbCompute;
    Semaphore semaphoreCompute;

    size_t currentFrame = 0;

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized);
  };

}  // namespace vkl