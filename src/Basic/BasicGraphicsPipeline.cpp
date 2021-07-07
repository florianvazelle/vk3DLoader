#include <Basic/BasicGraphicsPipeline.hpp>
#include <shadow_mapping_frag.h>            
#include <shadow_mapping_vert.h>            
#include <struct/Vertex.hpp>        
#include <stdexcept>                        
#include <vector>                        
#include <poike/poike.hpp>

using namespace poike;
using namespace vkl;

BasicGraphicsPipeline::BasicGraphicsPipeline(const Device& device,
                                             const SwapChain& swapChain,
                                             const RenderPass& renderPass,
                                             const DescriptorSetLayout& descriptorSetLayout)
    : GraphicsPipeline(device, swapChain, renderPass, descriptorSetLayout) {
  createPipeline();
}

BasicGraphicsPipeline::~BasicGraphicsPipeline() { destroyPipeline(); }

void BasicGraphicsPipeline::createPipeline() {
  {
    const VkDescriptorSetLayout layouts[]               = {m_descriptorSetLayout.handle()};
    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts    = layouts,
    };

    if (vkCreatePipelineLayout(m_device.logical(), &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
      throw std::runtime_error("Pipeline Layout creation failed");
    }
  }

  std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);

  VkPipelineVertexInputStateCreateInfo vertexInputInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssembly;
  VkPipelineViewportStateCreateInfo viewportState;
  VkPipelineRasterizationStateCreateInfo rasterizer;
  VkPipelineMultisampleStateCreateInfo multisampling;
  VkPipelineColorBlendStateCreateInfo colorBlending;
  VkPipelineDepthStencilStateCreateInfo depthStencil;

  initDefaultPipeline<Vertex>(vertexInputInfo, inputAssembly, viewportState, rasterizer, multisampling, colorBlending,
                              depthStencil);

  {
    const VkShaderModule vertShaderModule = createShaderModule(SHADOW_MAPPING_VERT);
    const VkShaderModule fragShaderModule = createShaderModule(SHADOW_MAPPING_FRAG);

    shaderStages[0] = misc::pipelineShaderStageCreateInfo(vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = misc::pipelineShaderStageCreateInfo(fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);

    const VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount          = 2,
        .pStages             = shaderStages.data(),
        .pVertexInputState   = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depthStencil,
        .pColorBlendState    = &colorBlending,
        .pDynamicState       = nullptr,
        .layout              = m_layout,
        .renderPass          = m_renderPass.handle(),
        // Pipeline will be used in second sub pass
        .subpass            = 0,  // 1,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = -1,
    };

    if (vkCreateGraphicsPipelines(m_device.logical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline)
        != VK_SUCCESS) {
      throw std::runtime_error("Graphics Pipeline creation failed");
    }
  }

  deleteShaderModule(shaderStages);
}