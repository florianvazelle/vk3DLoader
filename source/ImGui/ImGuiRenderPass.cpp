
#include <iostream>
#include <vkl/Device.hpp>
#include <vkl/ImGui/ImGuiRenderPass.hpp>
#include <vkl/SwapChain.hpp>

using namespace vkl;

ImGuiRenderPass::ImGuiRenderPass(const Device& device, const SwapChain& swapChain) : RenderPass(device, swapChain) {
  createRenderPass();
  createFrameBuffers();
}

void ImGuiRenderPass::createRenderPass() {
  // Create an attachment description for the render pass
  VkAttachmentDescription attachmentDescription = {
      .format         = m_swapChain.imageFormat(),
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Need UI to be drawn on top of main
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,  // Last pass so we want to present after
  };

  // Create a color attachment reference
  VkAttachmentReference attachmentReference = {
      .attachment = 0,
      .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  // Create a subpass
  VkSubpassDescription subpass = {
      .pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments    = &attachmentReference,
  };

  // Create a subpass dependency to synchronize our main and UI render passes
  // We want to render the UI after the geometry has been written to the framebuffer
  // so we need to configure a subpass dependency as such
  VkSubpassDependency subpassDependency = {
      .srcSubpass    = VK_SUBPASS_EXTERNAL,  // Create external dependency
      .dstSubpass    = 0,                    // The geometry subpass comes first
      .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // Wait on writes
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
  };

  // Finally create the UI render pass
  VkRenderPassCreateInfo createInfo = {
      .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments    = &attachmentDescription,
      .subpassCount    = 1,
      .pSubpasses      = &subpass,
      .dependencyCount = 1,
      .pDependencies   = &subpassDependency,
  };

  if (vkCreateRenderPass(m_device.logical(), &createInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
    throw std::runtime_error("Render pass creation failed");
  }
}
