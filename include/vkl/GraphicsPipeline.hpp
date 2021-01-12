
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

  protected:
    /**
     * Note Exposé : les VkPipeline peuvent partagé le meme VkPipelineLayout
     */
    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
    VkPipelineLayout m_oldLayout;

    // so that they are not destroyed in initDefaultPipeline
    VkVertexInputBindingDescription m_bindingDescription;
    std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    VkPipelineColorBlendAttachmentState m_colorBlendAttachment;

    const Device& m_device;
    const SwapChain& m_swapChain;
    const RenderPass& m_renderPass;
    const DescriptorSetLayout& m_descriptorSetLayout;

    void initDefaultPipeline(VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
                             VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
                             VkPipelineViewportStateCreateInfo& viewportState,
                             VkPipelineRasterizationStateCreateInfo& rasterizer,
                             VkPipelineMultisampleStateCreateInfo& multisampling,
                             VkPipelineColorBlendStateCreateInfo& colorBlending,
                             VkPipelineDepthStencilStateCreateInfo& depthStencil);
    virtual void createPipeline() = 0;

    VkShaderModule createShaderModule(const std::vector<unsigned char>& code);
    void deleteShaderModule(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages);
  };
}  // namespace vkl

#endif  // GRAPHICSPIPELINE_HPP