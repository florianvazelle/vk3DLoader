// clang-format off
#include <particle/Compute/ComputePipeline.hpp>
#include <particle_calculate_comp.h>         // for PARTICLE_CALCULATE_COMP
#include <particle_integrate_comp.h>         // for PARTICLE_INTEGRATE_COMP
#include <common/DescriptorSetLayout.hpp>    // for DescriptorSetLayout
#include <common/Device.hpp>                 // for Device
#include <common/GraphicsPipeline.hpp>       // for GraphicsPipeline
#include <common/SwapChain.hpp>              // for vkl
#include <common/misc/GraphicsPipeline.hpp>  // for pipelineShaderStageCreat...
#include <stdexcept>                         // for runtime_error
// clang-format on

using namespace vkl;

ComputePipeline::ComputePipeline(const Device& device,
                                 const SwapChain& swapChain,
                                 const RenderPass& renderPass,
                                 const DescriptorSetLayout& descriptorSetLayout)
    : GraphicsPipeline(device, swapChain, renderPass, descriptorSetLayout) {
  createPipeline();
}

void ComputePipeline::createPipeline() {
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

  VkComputePipelineCreateInfo computePipelineCreateInfo = {
      .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .flags  = 0,
      .layout = m_layout,
  };

  // 1st pass
  VkShaderModule compShaderModule = createShaderModule(PARTICLE_CALCULATE_COMP);
  computePipelineCreateInfo.stage = misc::pipelineShaderStageCreateInfo(compShaderModule, VK_SHADER_STAGE_COMPUTE_BIT);

  //   {
  //     // Set shader parameters via specialization constants
  //     struct SpecializationData {
  //       uint32_t sharedDataSize;
  //       float gravity;
  //       float power;
  //       float soften;
  //     } specializationData;

  //     std::vector<VkSpecializationMapEntry> specializationMapEntries;
  //     specializationMapEntries.push_back(vks::initializers::specializationMapEntry(0, offsetof(SpecializationData,
  //     sharedDataSize), sizeof(uint32_t))); specializationMapEntries.push_back(
  //     vks::initializers::specializationMapEntry(1, offsetof(SpecializationData, gravity), sizeof(float)));
  //     specializationMapEntries.push_back( vks::initializers::specializationMapEntry(2, offsetof(SpecializationData,
  //     power), sizeof(float))); specializationMapEntries.push_back(vks::initializers::specializationMapEntry(3,
  //     offsetof(SpecializationData, soften), sizeof(float)));

  //     specializationData.sharedDataSize = std::min((uint32_t)1024,
  //     (uint32_t)(vulkanDevice->properties.limits.maxComputeSharedMemorySize / sizeof(glm::vec4)));

  //     specializationData.gravity = 0.002f;
  //     specializationData.power   = 0.75f;
  //     specializationData.soften  = 0.05f;

  //     VkSpecializationInfo specializationInfo = vks::initializers::specializationInfo(
  //     static_cast<uint32_t>(specializationMapEntries.size()), specializationMapEntries.data(),
  //     sizeof(specializationData), &specializationData); computePipelineCreateInfo.stage.pSpecializationInfo =
  //     &specializationInfo;
  //   }

  if (vkCreateComputePipelines(m_device.logical(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr,
                               &m_pipelineCalculate)
      == VK_SUCCESS) {
    throw std::runtime_error("Compute Pipeline creation failed");
  }

  // 2nd pass
  compShaderModule                = createShaderModule(PARTICLE_INTEGRATE_COMP);
  computePipelineCreateInfo.stage = misc::pipelineShaderStageCreateInfo(compShaderModule, VK_SHADER_STAGE_COMPUTE_BIT);

  if (vkCreateComputePipelines(m_device.logical(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr,
                               &m_pipelineIntegrate)
      == VK_SUCCESS) {
    throw std::runtime_error("Compute Pipeline creation failed");
  }
}