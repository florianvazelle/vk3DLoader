#include <iostream>
#include <vkl/Basic/BasicRenderPass.hpp>
#include <vkl/Device.hpp>
#include <vkl/SwapChain.hpp>

using namespace vkl;

BasicRenderPass::BasicRenderPass(const Device& device, const SwapChain& swapChain) : RenderPass(device, swapChain) {
  createRenderPass();
  createFrameBuffers();
}

void BasicRenderPass::createRenderPass() {
  // Create a new render pass as a color attachment
  const VkAttachmentDescription colorAttachment = {
      // Format should match the format of the swap chain
      .format = m_swapChain.imageFormat(),
      // No multisampling
      .samples = VK_SAMPLE_COUNT_1_BIT,
      // Clear data before rendering, then store result after
      .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      // Not doing anything with stencils, so don't care about it
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      // Don't care about initial layout, but final layout should
      // be same as the presentation source
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  /**
   * Note Exposé : Les subpasses sont des opérations de rendu dépendant du contenu présent dans le
   * framebuffer quand elles commencent. Elles peuvent consister en des opérations de
   * post-processing exécutées l'une après l'autre. En regroupant toutes ces opérations en une seule
   * passe, Vulkan peut alors réaliser des optimisations et conserver de la bande passante pour de
   * potentiellement meilleures performances.
   */

  /* Post-rendering subpasses */

  // Subpass attachment reference
  const VkAttachmentReference colorRef = {
      // Index of 0 in color attachments description array (we're only using 1)
      .attachment = 0,
      // Use the optimal layout for color attachments
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  // Subpass description
  const VkSubpassDescription subpass = {
      // Using for graphics computation
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      // Attach the color attachment
      .colorAttachmentCount = 1,
      .pColorAttachments    = &colorRef,
  };

  // Subpass dependencies
  const VkSubpassDependency dependency = {
      // Implicit subpass
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      // Subpass depdency is in index 0
      .dstSubpass = 0,
      // Wait for color attachment output before accessing image
      // This prevents the image being accessed by subpass and swap chain at the same time
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      // Prevent transistion from happening until after reading and writing of color attachment
      .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
  };

  // Create the render pass
  const VkRenderPassCreateInfo createInfo = {
      .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments    = &colorAttachment,
      .subpassCount    = 1,
      .pSubpasses      = &subpass,
      .dependencyCount = 1,
      .pDependencies   = &dependency,
  };

  if (vkCreateRenderPass(m_device.logical(), &createInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
    throw std::runtime_error("Render pass creation failed");
  }
}