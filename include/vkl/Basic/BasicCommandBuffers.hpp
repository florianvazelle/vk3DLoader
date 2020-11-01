#ifndef BASICCOMMANDBUFFERS_HPP
#define BASICCOMMANDBUFFERS_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <vkl/CommandBuffers.hpp>

namespace vkl {

  class BasicCommandBuffers : public CommandBuffers {
  public:
    BasicCommandBuffers(const Device& device,
                        const RenderPass& renderpass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool);
    void recreate();

  private:
    void createCommandBuffers();
  };

}  // namespace vkl

#endif  // BASICCOMMANDBUFFERS_HPP