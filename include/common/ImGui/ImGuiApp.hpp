
#ifndef IMGUIAPP_HPP
#define IMGUIAPP_HPP

#include <vulkan/vulkan.h>

#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <common/CommandPool.hpp>
#include <common/Device.hpp>
#include <common/ImGui/ImGuiCommandBuffers.hpp>
#include <common/ImGui/ImGuiRenderPass.hpp>
#include <common/Instance.hpp>
#include <common/SwapChain.hpp>
#include <common/Window.hpp>

namespace vkl {

  class ImGuiApp {
  public:
    ImGuiApp(const Instance& instance,
             Window& window,
             const Device& device,
             const SwapChain& swapChain,
             const GraphicsPipeline& graphicsPipeline);
    ~ImGuiApp();

    inline VkCommandBuffer& command(uint32_t index) { return commandBuffers.command(index); }
    inline const VkCommandBuffer& command(uint32_t index) const { return commandBuffers.command(index); }

    void recordCommandBuffers(uint32_t bufferIdx) { commandBuffers.recordCommandBuffers(bufferIdx); }

    void recreate();

  private:
    VkDescriptorPool imGuiDescriptorPool;

    ImGuiRenderPass renderPass;
    CommandPool commandPool;
    ImGuiCommandBuffers commandBuffers;

    const Instance& m_instance;
    const Device& m_device;
    const SwapChain& m_swapChain;
    const GraphicsPipeline& m_graphicsPipeline;

    void createImGuiDescriptorPool();
  };
}  // namespace vkl

#endif  // IMGUIAPP_HPP