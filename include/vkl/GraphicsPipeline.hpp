
#ifndef GRAPHICSPIPELINE_HPP
#define GRAPHICSPIPELINE_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <vector>

namespace vkl {

  class Device;
  class SwapChain;
  class RenderPass;
  class DescriptorSetLayout;
  struct ShaderDetails;

  class GraphicsPipeline : public NonCopyable {
  public:
    GraphicsPipeline(const Device& device,
                     const SwapChain& swapChain,
                     const RenderPass& renderPass,
                     const DescriptorSetLayout& descriptorSetLayout);
    ~GraphicsPipeline();

    void recreate();

    inline const VkPipeline& pipeline() const { return m_pipeline; }
    inline const VkPipelineLayout& layout() const { return m_layout; }

  private:
    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
    VkPipelineLayout m_oldLayout;

    const Device& m_device;
    const SwapChain& m_swapChain;
    const RenderPass& m_renderPass;
    const DescriptorSetLayout& m_descriptorSetLayout;

    void createPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);
  };
}  // namespace vkl

#endif  // GRAPHICSPIPELINE_HPP