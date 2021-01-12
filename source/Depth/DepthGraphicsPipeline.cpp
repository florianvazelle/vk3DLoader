#include <vkl/Depth/DepthGraphicsPipeline.hpp>

#include <depth_basic_vert.h>

#include <iostream>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/Device.hpp>
#include <vkl/RenderPass.hpp>
#include <vkl/ShaderLoader.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/misc/GraphicsPipeline.hpp>
#include <vkl/struct/Vertex.hpp>

using namespace vkl;

DepthGraphicsPipeline::DepthGraphicsPipeline(const Device& device,
                                             const SwapChain& swapChain,
                                             const RenderPass& renderPass,
                                             const DescriptorSetLayout& descriptorSetLayout)
    : GraphicsPipeline(device, swapChain, renderPass, descriptorSetLayout) {
  createPipeline();
}

void DepthGraphicsPipeline::createPipeline() {
  // Creation Layout
  // TODO : made a separated method
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

  initDefaultPipeline(vertexInputInfo, inputAssembly, viewportState, rasterizer, multisampling, colorBlending,
                      depthStencil);

  {
    /* Depth pipeline (vertex shader only) */

    const VkShaderModule vertShaderModule = createShaderModule(DEPTH_BASIC_VERT);
    shaderStages[0] = misc::pipelineShaderStageCreateInfo(vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);

    // Enable depth bias
    rasterizer.depthBiasEnable = VK_TRUE;

    // No blend attachment states (no color attachments used)
    colorBlending.attachmentCount = 0;

    // Add depth bias to dynamic state, so we can change it at runtime
    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_DEPTH_BIAS};

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size()),
        .pDynamicStates    = dynamicStateEnables.data(),
    };

    const VkGraphicsPipelineCreateInfo offscreenInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount          = 1,
        .pStages             = shaderStages.data(),
        .pVertexInputState   = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depthStencil,
        .pColorBlendState    = &colorBlending,
        .pDynamicState       = &pipelineDynamicStateCreateInfo,
        .layout              = m_layout,
        .renderPass          = m_renderPass.handle(),
        // Pipeline will be used in first sub pass
        .subpass            = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = -1,
    };

    if (vkCreateGraphicsPipelines(m_device.logical(), VK_NULL_HANDLE, 1, &offscreenInfo, nullptr, &m_pipeline)
        != VK_SUCCESS) {
      throw std::runtime_error("Depth Graphics Pipeline creation failed");
    }
  }

  deleteShaderModule(shaderStages);
}