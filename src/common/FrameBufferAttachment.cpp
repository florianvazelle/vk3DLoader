// clang-format off
#include <common/FrameBufferAttachment.hpp>
#include <stdint.h>                // for uint32_t
#include <common/misc/Device.hpp>  // for findMemoryType, formatIsFilterable
#include <stdexcept>               // for runtime_error
#include <common/Device.hpp>       // for Device
#include <common/QueueFamily.hpp>  // for vkl
#include <common/SwapChain.hpp>    // for SwapChain
// clang-format on

using namespace vkl;

/**
 * Image qui va partir dans le shader
 */
FrameBufferAttachment::FrameBufferAttachment(const Device& device,
                                             const SwapChain& swapChain,
                                             VkFormat format,
                                             VkImageUsageFlags usage)
    : m_device(device), m_swapChain(swapChain), m_format(format), m_sampler(VK_NULL_HANDLE) {
  VkImageAspectFlags aspectMask = 0;
  VkImageLayout imageLayout;

  if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
    aspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;
    imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    aspectMask  = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }

  const VkImageCreateInfo imageInfo = {
      .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType     = VK_IMAGE_TYPE_2D,
      .format        = format,
      .extent        = {
        .width  = m_swapChain.extent().width,
        .height = m_swapChain.extent().height,
        .depth  = 1,
      },
      .mipLevels     = 1,
      .arrayLayers   = 1,
      .samples       = VK_SAMPLE_COUNT_1_BIT,
      .tiling        = VK_IMAGE_TILING_OPTIMAL,
      // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT flag is required for input attachments,
      .usage         = usage, // | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
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
    throw std::runtime_error("vkBindImageMemory failed");
  }

  const VkImageViewCreateInfo imageViewInfo = {
      .sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image                           = m_image,
      .viewType                        = VK_IMAGE_VIEW_TYPE_2D,
      .format                          = format,
      .subresourceRange                = {
          .aspectMask     = aspectMask,
          .baseMipLevel   = 0,
          .levelCount     = 1,
          .baseArrayLayer = 0,
          .layerCount     = 1,
      },
  };

  if (vkCreateImageView(m_device.logical(), &imageViewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
    throw std::runtime_error("vkCreateImageView failed");
  }
}

void FrameBufferAttachment::createSample() {
  // Create sampler to sample from to depth attachment
  // Used to sample in the fragment shader for shadowed rendering
  const VkFilter filter = misc::formatIsFilterable(m_device.physical(), m_format, VK_IMAGE_TILING_OPTIMAL,
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
                              ? VK_FILTER_LINEAR
                              : VK_FILTER_NEAREST;

  const VkSamplerCreateInfo sampler = {
      .sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter     = filter,
      .minFilter     = filter,
      .mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeW  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .mipLodBias    = 0.0f,
      .maxAnisotropy = 1.0f,
      .minLod        = 0.0f,
      .maxLod        = 1.0f,
      .borderColor   = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
  };

  if (vkCreateSampler(m_device.logical(), &sampler, nullptr, &m_sampler) != VK_SUCCESS) {
    throw std::runtime_error("Sampler creation failed");
  }
}

FrameBufferAttachment ::~FrameBufferAttachment() {
  vkDestroyImage(m_device.logical(), m_image, nullptr);
  vkDestroyImageView(m_device.logical(), m_imageView, nullptr);
  vkFreeMemory(m_device.logical(), m_bufferMemory, nullptr);


  if (m_sampler != VK_NULL_HANDLE) vkDestroySampler(m_device.logical(), m_sampler, nullptr);
}
