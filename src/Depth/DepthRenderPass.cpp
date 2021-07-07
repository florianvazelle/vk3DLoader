#include <Depth/DepthRenderPass.hpp>
#include <stddef.h>                        
#include <stdint.h>                         
#include <array>                          
#include <memory>                            
#include <stdexcept>                      
#include <vector>                
#include <poike/poike.hpp>

using namespace poike;
using namespace vkl;

DepthRenderPass::DepthRenderPass(const Device& device, const SwapChain& swapChain) : RenderPass(device, swapChain) {
  createRenderPass();
  createFrameBuffers();
}

void DepthRenderPass::createRenderPass() {
  const VkFormat depthFormat = misc::findDepthFormat(m_device.physical());

  const VkAttachmentDescription depthAttachment = {
      .format         = depthFormat,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,  // Attachment will be transitioned to
                                                                          // shader read at render pass end,
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
      .pAttachments    = &depthAttachment,
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
  const size_t numImages     = m_swapChain.numImages();
  const VkFormat depthFormat = misc::findDepthFormat(m_device.physical());

  // Fill attachments for one depth attachment by frame
  m_depthAttachments.resize(numImages);
  for (size_t i = 0; i < numImages; i++) {
    m_depthAttachments[i] = std::make_unique<Attachment>(m_device, m_swapChain, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
  }

  m_frameBuffers.resize(numImages);

  // Initialize default framebuffer creation info
  VkImageView attachments[1];
  const VkFramebufferCreateInfo info = {
      .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass      = m_renderPass,
      .attachmentCount = 1,
      .pAttachments    = attachments,
      .width           = m_swapChain.extent().width,
      .height          = m_swapChain.extent().height,
      .layers          = 1,
  };

  // Create a framebuffer for each image view
  for (size_t i = 0; i < numImages; ++i) {
    attachments[0] = m_depthAttachments[i]->view();

    if (vkCreateFramebuffer(m_device.logical(), &info, nullptr, &m_frameBuffers.at(i)) != VK_SUCCESS) {
      throw std::runtime_error("Framebuffer creation failed");
    }
  }
}
