#ifndef IMGUICOMMANDBUFFERS_HPP
#define IMGUICOMMANDBUFFERS_HPP

#include <vulkan/vulkan.h>
#include <common/CommandBuffers.hpp>
#include <vector>

namespace vkl {

  class ImGuiCommandBuffers : public CommandBuffers {
  public:
    ImGuiCommandBuffers(const Device& device,
                        const RenderPass& renderpass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool);

    void recordCommandBuffers(uint32_t bufferIdx);
    void recreate();

  private:
    void createCommandBuffers();
  };

}  // namespace vkl

#endif  // IMGUICOMMANDBUFFERS_HPP