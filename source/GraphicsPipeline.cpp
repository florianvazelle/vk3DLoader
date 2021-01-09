#include <vkl/GraphicsPipeline.hpp>

#include <depth_basic_vert.h>
#include <shadow_mapping_frag.h>
#include <shadow_mapping_vert.h>

#include <iostream>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/Device.hpp>
#include <vkl/RenderPass.hpp>
#include <vkl/ShaderLoader.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/misc/GraphicsPipeline.hpp>
#include <vkl/struct/Vertex.hpp>

using namespace vkl;

GraphicsPipeline::GraphicsPipeline(const Device& device,
                                   const SwapChain& swapChain,
                                   const RenderPass& renderPass,
                                   const RenderPass& depthRenderPass,
                                   const DescriptorSetLayout& descriptorSetLayout)
    : m_pipeline(VK_NULL_HANDLE),
      m_depthPipeline(VK_NULL_HANDLE),
      m_layout(VK_NULL_HANDLE),
      m_oldLayout(VK_NULL_HANDLE),

      m_device(device),
      m_swapChain(swapChain),
      m_renderPass(renderPass),
      m_depthRenderPass(depthRenderPass),
      m_descriptorSetLayout(descriptorSetLayout) {
  createPipeline();
}

GraphicsPipeline::~GraphicsPipeline() {
  vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
  vkDestroyPipeline(m_device.logical(), m_depthPipeline, nullptr);
  vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
}

void GraphicsPipeline::recreate() {
  vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
  vkDestroyPipeline(m_device.logical(), m_depthPipeline, nullptr);
  vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
  createPipeline();
}

void GraphicsPipeline::createPipeline() {
  VkPipelineShaderStageCreateInfo shaderStages[2];

  const auto bindingDescription    = Vertex::getBindingDescription();
  const auto attributeDescriptions = Vertex::getAttributeDescriptions();

  const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
      .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount   = 1,
      .pVertexBindingDescriptions      = &bindingDescription,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
      .pVertexAttributeDescriptions    = attributeDescriptions.data(),
  };

  // Create information struct about input assembly
  // We'll be organizing our vertices in triangles and the GPU should treat the data accordingly
  const VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
      .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  // Pipeline viewport
  const VkViewport viewport = {
      .x      = 0.0f,
      .y      = 0.0f,
      .width  = static_cast<float>(m_swapChain.extent().width),
      .height = static_cast<float>(m_swapChain.extent().height),
      // Depth buffer range
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  // Pixel boundary cutoff
  const VkRect2D scissor = {
      .offset = {0, 0},
      .extent = m_swapChain.extent(),
  };

  // Combine viewport(s) and scissor(s) (some graphics cards allow multiple of each)
  const VkPipelineViewportStateCreateInfo viewportState = {
      .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports    = &viewport,
      .scissorCount  = 1,
      .pScissors     = &scissor,
  };

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      // Clip fragments instead of clipping them to near and far planes
      .depthClampEnable = VK_FALSE,
      // Don't allow the rasterizer to discard geometry
      .rasterizerDiscardEnable = VK_FALSE,
      // Fill fragments
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode    = VK_CULL_MODE_BACK_BIT,
      .frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE,
      // Bias depth values
      // This is good for shadow mapping, but we're not doing that currently
      // so we'll disable for now
      .depthBiasEnable = VK_FALSE,
      .lineWidth       = 1.0f,
  };

  // Multisampling
  const VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable  = VK_FALSE,
      .minSampleShading     = 1.0f,
  };

  // Color Blending
  const VkPipelineColorBlendAttachmentState colorBlendAttachment = {
      // Disable blending
      .blendEnable = VK_FALSE,
      .colorWriteMask
      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo colorBlending = {
      .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments    = &colorBlendAttachment,
  };

  VkPipelineDepthStencilStateCreateInfo depthStencil = {
        .sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable  = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL,  // Cull front faces
        .stencilTestEnable = VK_FALSE,
        .back = {
            .compareOp   = VK_COMPARE_OP_ALWAYS,
        },
    };

  const VkDescriptorSetLayout layouts[]               = {m_descriptorSetLayout.handle()};
  const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
      .sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts    = layouts,
  };

  if (vkCreatePipelineLayout(m_device.logical(), &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
    throw std::runtime_error("Pipeline Layout creation failed");
  }

  {
    // Load our shader modules in from disk
    const auto vertShaderCode = SHADOW_MAPPING_VERT;  // ShaderLoader::load(DATA_PATH "/shader/base.vert.spv");
    const auto fragShaderCode = SHADOW_MAPPING_FRAG;  // ShaderLoader::load(DATA_PATH "/shader/base.frag.spv");

    const VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    const VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    const VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName  = "main",
    };

    // Push constant
    // VkSpecializationMapEntry specializationMapEntry = {
    //     .constantID = 0,
    //     .offset     = 0,
    //     .size       = sizeof(m_enabledShadowMap),
    // };

    // VkSpecializationInfo specializationInfo = {
    //     .mapEntryCount = 1,
    //     .pMapEntries   = &specializationMapEntry,
    //     .dataSize      = sizeof(m_enabledShadowMap),
    //     .pData         = &m_enabledShadowMap,
    // };

    const VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName  = "main",
        // .pSpecializationInfo = &specializationInfo,
    };

    shaderStages[0] = vertShaderStageInfo;
    shaderStages[1] = fragShaderStageInfo;

    const VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount          = 2,
        .pStages             = shaderStages,
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

  {
    /* Offscreen pipeline (vertex shader only) */

    const VkShaderModule vertDepthShaderModule = createShaderModule(DEPTH_BASIC_VERT);

    const VkPipelineShaderStageCreateInfo vertDepthShaderStageInfo = {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertDepthShaderModule,
        .pName  = "main",
    };

    shaderStages[0] = vertDepthShaderStageInfo;

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
        .pStages             = shaderStages,
        .pVertexInputState   = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depthStencil,
        .pColorBlendState    = &colorBlending,
        .pDynamicState       = &pipelineDynamicStateCreateInfo,
        .layout              = m_layout,
        .renderPass          = m_depthRenderPass.handle(),
        // Pipeline will be used in first sub pass
        .subpass            = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = -1,
    };

    if (vkCreateGraphicsPipelines(m_device.logical(), VK_NULL_HANDLE, 1, &offscreenInfo, nullptr, &m_depthPipeline)
        != VK_SUCCESS) {
      throw std::runtime_error("Depth Graphics Pipeline creation failed");
    }
  }

  // Delete shader modules
  for (auto& shader : shaderStages) {
    vkDestroyShaderModule(m_device.logical(), shader.module, nullptr);
  }
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<unsigned char>& code) {
  const VkShaderModuleCreateInfo createInfo = {
      .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code.size(),
      // Vector class already ensures that the data is correctly aligned,
      // so no need to manually do it
      .pCode = reinterpret_cast<const uint32_t*>(code.data()),
  };

  VkShaderModule module;
  if (vkCreateShaderModule(m_device.logical(), &createInfo, nullptr, &module) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return module;
}