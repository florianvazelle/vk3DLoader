
#include <iostream>
#include <vkl/Device.hpp>
#include <vkl/RenderPass.hpp>
#include <vkl/SwapChain.hpp>

using namespace vkl;

RenderPass::RenderPass(const Device& device, const SwapChain& swapChain)
    : m_renderPass(VK_NULL_HANDLE), m_oldRenderPass(VK_NULL_HANDLE), m_device(device), m_swapChain(swapChain) {}

RenderPass::~RenderPass() {
  destroyFrameBuffers();
  vkDestroyRenderPass(m_device.logical(), m_renderPass, nullptr);
}

void RenderPass::recreate() {
  destroyFrameBuffers();
  m_oldRenderPass = m_renderPass;
  createRenderPass();
  createFrameBuffers();
}

void RenderPass::cleanupOld() {
  if (m_oldRenderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(m_device.logical(), m_oldRenderPass, nullptr);
    m_oldRenderPass = VK_NULL_HANDLE;
  }
}

void RenderPass::createFrameBuffers() {
  const size_t numImages = 1;  // m_swapChain.numImages();

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