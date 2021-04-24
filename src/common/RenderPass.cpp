
#include <common/Device.hpp>
#include <common/RenderPass.hpp>
#include <common/SwapChain.hpp>
#include <iostream>

using namespace vkl;

RenderPass::RenderPass(const Device& device, const SwapChain& swapChain)
    : m_renderPass(VK_NULL_HANDLE), m_device(device), m_swapChain(swapChain) {}

RenderPass::~RenderPass() {
  destroyFrameBuffers();
  vkDestroyRenderPass(m_device.logical(), m_renderPass, nullptr);
}

void RenderPass::recreate() {
  destroyFrameBuffers();
  vkDestroyRenderPass(m_device.logical(), m_renderPass, nullptr);

  createRenderPass();
  createFrameBuffers();
}

void RenderPass::createFrameBuffers() {
  const size_t numImages = m_swapChain.numImages();

  m_frameBuffers.resize(numImages);

  // Create a framebuffer for each image view
  for (size_t i = 0; i < numImages; ++i) {
    const VkImageView attachments[] = {m_swapChain.imageView(i)};

    const VkFramebufferCreateInfo info = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass      = m_renderPass,
        .attachmentCount = 1,
        .pAttachments    = attachments,
        .width           = m_swapChain.extent().width,
        .height          = m_swapChain.extent().height,
        .layers          = 1,
    };

    if (vkCreateFramebuffer(m_device.logical(), &info, nullptr, &m_frameBuffers.at(i)) != VK_SUCCESS) {
      throw std::runtime_error("Framebuffer creation failed");
    }
  }
}

void RenderPass::destroyFrameBuffers() {
  for (VkFramebuffer& fb : m_frameBuffers) {
    vkDestroyFramebuffer(m_device.logical(), fb, nullptr);
  }
}