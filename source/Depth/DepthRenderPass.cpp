#include <vkl/Depth/DepthRenderPass.hpp>

#include <array>
#include <iostream>
#include <vkl/Device.hpp>
#include <vkl/SwapChain.hpp>
#include <vkl/misc/Device.hpp>

#ifndef SHADOWMAP_DIM
#  define SHADOWMAP_DIM 2048
#endif

using namespace vkl;

DepthRenderPass::DepthRenderPass(const Device& device, const SwapChain& swapChain) : RenderPass(device, swapChain) {
  createRenderPass();
  createFrameBuffers();
}

void DepthRenderPass::createRenderPass() {
  const VkAttachmentDescription colorAttachment = {
      .format         = VK_FORMAT_D16_UNORM,  // Depth Format
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,  // Attachment will be transitioned to shader read
                                                                       // at render pass end
  };

  const VkAttachmentReference depthReference = {
      .attachment = 0,
      .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,  // Attachment will be used as depth/stencil
                                                                       // during render pass
  };

  // Subpass description
  const VkSubpassDescription subpass = {
      .pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 0,  // No color attachments
      // Reference to our depth attachment
      .pDepthStencilAttachment = &depthReference,
  };

  // Use subpass dependencies for layout transitions
  std::array<VkSubpassDependency, 2> dependencies;

  dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass      = 0;
  dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependencies[0].srcAccessMask   = VK_ACCESS_SHADER_READ_BIT;
  dependencies[0].dstAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  dependencies[1].srcSubpass      = 0;
  dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
  dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependencies[1].srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependencies[1].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
  dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  // Create the render pass
  const VkRenderPassCreateInfo createInfo = {
      .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments    = &colorAttachment,
      .subpassCount    = 1,
      .pSubpasses      = &subpass,
      .dependencyCount = static_cast<uint32_t>(dependencies.size()),
      .pDependencies   = dependencies.data(),
  };

  if (vkCreateRenderPass(m_device.logical(), &createInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
    throw std::runtime_error("Render pass creation failed");
  }
}

void DepthRenderPass::createFrameBuffers() {
  // For shadow mapping we only need a depth attachment
  const VkImageCreateInfo imageInfo = {
      .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType     = VK_IMAGE_TYPE_2D,
      .format        = VK_FORMAT_D16_UNORM,  // Depth stencil attachment
      .extent        = {
        .width  = SHADOWMAP_DIM,
        .height = SHADOWMAP_DIM,
        .depth  = 1,
      },
      .mipLevels     = 1,
      .arrayLayers   = 1,
      .samples       = VK_SAMPLE_COUNT_1_BIT,
      .tiling        = VK_IMAGE_TILING_OPTIMAL,
      .usage
      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
        | VK_IMAGE_USAGE_SAMPLED_BIT,  // We will sample directly from the depth attachment for the shadow mapping
  };

  if (vkCreateImage(m_device.logical(), &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
    throw std::runtime_error("vkCreateImage failed");
  }

  VkMemoryRequirements memReqs;
  // Returns the memory requirements for specified Vulkan object
  vkGetImageMemoryRequirements(m_device.logical(), m_image, &memReqs);

  uint32_t memTypeIndex
      = misc::findMemoryType(m_device.physical(), memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  const VkMemoryAllocateInfo memAlloc = {
      .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize  = memReqs.size,
      .memoryTypeIndex = memTypeIndex,
  };

  if (vkAllocateMemory(m_device.logical(), &memAlloc, nullptr, &m_bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("vkAllocateMemory failed");
  }

  if (vkBindImageMemory(m_device.logical(), m_image, m_bufferMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("RvkBindImageMemory failed");
  }

  const VkImageViewCreateInfo depthStencilView = {
      .sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image                           = m_image,
      .viewType                        = VK_IMAGE_VIEW_TYPE_2D,
      .format                          = VK_FORMAT_D16_UNORM,
      .subresourceRange                = {
          .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
          .baseMipLevel   = 0,
          .levelCount     = 1,
          .baseArrayLayer = 0,
          .layerCount     = 1,
      },
  };

  if (vkCreateImageView(m_device.logical(), &depthStencilView, nullptr, &m_imageView) != VK_SUCCESS) {
    throw std::runtime_error("vkCreateImageView failed");
  }

  // Create sampler to sample from to depth attachment
  // Used to sample in the fragment shader for shadowed rendering
  const VkFilter shadowmap_filter
      = misc::formatIsFilterable(m_device.physical(), VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL) ? VK_FILTER_LINEAR
                                                                                                    : VK_FILTER_NEAREST;

  const VkSamplerCreateInfo sampler = {
      .sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter     = shadowmap_filter,
      .minFilter     = shadowmap_filter,
      .mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV  = sampler.addressModeU,
      .addressModeW  = sampler.addressModeU,
      .mipLodBias    = 0.0f,
      .maxAnisotropy = 1.0f,
      .minLod        = 0.0f,
      .maxLod        = 1.0f,
      .borderColor   = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
  };

  if (vkCreateSampler(m_device.logical(), &sampler, nullptr, &m_depthSampler) != VK_SUCCESS) {
    throw std::runtime_error("Depth sampler creation failed");
  }

  m_frameBuffers.resize(1);

  // Create frame buffer
  const VkFramebufferCreateInfo frameBufferInfo = {
      .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass      = m_renderPass,
      .attachmentCount = 1,
      .pAttachments    = &m_imageView,
      .width           = SHADOWMAP_DIM,
      .height          = SHADOWMAP_DIM,
      .layers          = 1,
  };

  if (vkCreateFramebuffer(m_device.logical(), &frameBufferInfo, nullptr, &m_frameBuffers[0]) != VK_SUCCESS) {
    throw std::runtime_error("Depth FrameBuffer creation failed");
  }
}