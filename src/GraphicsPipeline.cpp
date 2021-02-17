#include <vkl/GraphicsPipeline.hpp>

#include <depth_basic_vert.h>
#include <shadow_mapping_frag.h>
#include <shadow_mapping_vert.h>

#include <iostream>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/Device.hpp>
#include <vkl/RenderPass.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/misc/GraphicsPipeline.hpp>
#include <vkl/struct/Vertex.hpp>

using namespace vkl;

GraphicsPipeline::GraphicsPipeline(const Device& device,
                                   const SwapChain& swapChain,
                                   const RenderPass& renderPass,
                                   const DescriptorSetLayout& descriptorSetLayout)
    : m_pipeline(VK_NULL_HANDLE),
      m_layout(VK_NULL_HANDLE),
      m_oldLayout(VK_NULL_HANDLE),

      m_device(device),
      m_swapChain(swapChain),
      m_renderPass(renderPass),
      m_descriptorSetLayout(descriptorSetLayout) {}

GraphicsPipeline::~GraphicsPipeline() {
  vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
  vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
}

void GraphicsPipeline::recreate() {
  vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
  vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
  createPipeline();
}

void GraphicsPipeline::initDefaultPipeline(VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
                                           VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
                                           VkPipelineViewportStateCreateInfo& viewportState,
                                           VkPipelineRasterizationStateCreateInfo& rasterizer,
                                           VkPipelineMultisampleStateCreateInfo& multisampling,
                                           VkPipelineColorBlendStateCreateInfo& colorBlending,
                                           VkPipelineDepthStencilStateCreateInfo& depthStencil) {
  /**
   * Vertex Input Info
   */
  m_bindingDescription    = Vertex::getBindingDescription();
  m_attributeDescriptions = Vertex::getAttributeDescriptions();

  vertexInputInfo = {
      .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount   = 1,
      .pVertexBindingDescriptions      = &m_bindingDescription,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size()),
      .pVertexAttributeDescriptions    = m_attributeDescriptions.data(),
  };

  /**
   * On spécifie comment le GPU doit assemblée les données en entré
   * Ici on organise nos sommets en triangles
   */
  inputAssembly = {
      .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  /**
   * Viewport
   */

  // Pipeline viewport
  m_viewport = {
      .x      = 0.0f,
      .y      = 0.0f,
      .width  = static_cast<float>(m_swapChain.extent().width),
      .height = static_cast<float>(m_swapChain.extent().height),
      // Depth buffer range
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  // Pixel boundary cutoff
  m_scissor = {
      .offset = {0, 0},
      .extent = m_swapChain.extent(),
  };

  // Combine viewport(s) and scissor(s) (some graphics cards allow multiple of each)
  viewportState = {
      .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports    = &m_viewport,
      .scissorCount  = 1,
      .pScissors     = &m_scissor,
  };

  /**
   * Rasterizer
   */
  rasterizer = {
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

  /**
   * Multisampling
   */
  multisampling = {
      .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable  = VK_FALSE,
      .minSampleShading     = 1.0f,
  };

  /**
   * Color Blending
   */
  m_colorBlendAttachment = {
      // Disable blending
      .blendEnable = VK_FALSE,
      .colorWriteMask
      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  colorBlending = {
      .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments    = &m_colorBlendAttachment,
  };

  /**
   * Depth Stencil
   */
  depthStencil = {
        .sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable  = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL,  // Cull front faces
        .stencilTestEnable = VK_FALSE,
        .back = {
            .compareOp   = VK_COMPARE_OP_ALWAYS,
        },
    };
}

// TODO : maybe externilize shader module
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

void GraphicsPipeline::deleteShaderModule(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages) {
  for (auto& shader : shaderStages) {
    vkDestroyShaderModule(m_device.logical(), shader.module, nullptr);
  }
}