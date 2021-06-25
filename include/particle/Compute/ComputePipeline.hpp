
#ifndef COMPUTEPIPELINE_HPP
#define COMPUTEPIPELINE_HPP

// clang-format off
#include <common/VulkanHeader.hpp>         // for VkPipeline, VkPipeline_T
#include <common/GraphicsPipeline.hpp>  // for GraphicsPipeline
namespace vkl { class DescriptorSetLayout; }
namespace vkl { class Device; }
namespace vkl { class RenderPass; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {

  class ComputePipeline : public GraphicsPipeline {
  public:
    ComputePipeline(const Device& device,
                    const SwapChain& swapChain,
                    const RenderPass& renderPass,
                    const DescriptorSetLayout& descriptorSetLayout);
    ~ComputePipeline();

    void recreate() final;

    inline const VkPipeline& pipelineCalculate() const { return m_pipelineCalculate; }
    inline const VkPipeline& pipelineIntegrate() const { return m_pipelineIntegrate; }

  private:
    VkPipeline m_pipelineCalculate, m_pipelineIntegrate;

    void createPipeline() final;
    void destroyComputePipeline();
  };
}  // namespace vkl

#endif  // COMPUTEPIPELINE_HPP