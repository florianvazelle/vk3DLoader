#include <common/ImGui/ImGuiApp.hpp>

using namespace vkl;

ImGuiApp::ImGuiApp(const Instance& instance,
                   Window& window,
                   const Device& device,
                   const SwapChain& swapChain,
                   const GraphicsPipeline& graphicsPipeline)
    : m_instance(instance),
      m_device(device),
      m_swapChain(swapChain),
      m_graphicsPipeline(graphicsPipeline),
      renderPass(device, swapChain),
      commandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
      commandBuffers(device, renderPass, swapChain, graphicsPipeline, commandPool),
      imGuiDescriptorPool(VK_NULL_HANDLE) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Initialize some DearImgui specific resources
  createImGuiDescriptorPool();

  QueueFamilyIndices indices = QueueFamily::FindQueueFamilies(m_device.physical(), window.surface());

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForVulkan(window.window(), true);
  ImGui_ImplVulkan_InitInfo init_info = {
      .Instance       = instance.handle(),
      .PhysicalDevice = m_device.physical(),
      .Device         = m_device.logical(),
      .QueueFamily    = indices.graphicsFamily.value(),
      .Queue          = m_device.graphicsQueue(),
      .DescriptorPool = imGuiDescriptorPool,
      .MinImageCount  = static_cast<uint32_t>(swapChain.numImages()),
      .ImageCount     = static_cast<uint32_t>(swapChain.numImages()),
  };
  ImGui_ImplVulkan_Init(&init_info, renderPass.handle());

  CommandBuffers::SingleTimeCommands(m_device, commandPool, [](const VkCommandBuffer& commandBuffer) {
    // Upload the fonts for DearImgui
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
  });
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

ImGuiApp::~ImGuiApp() {
  // Resources to destroy when the program ends
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  vkDestroyDescriptorPool(m_device.logical(), imGuiDescriptorPool, nullptr);
}

void ImGuiApp::recreate() {
  renderPass.recreate();
  commandBuffers.recreate();
};

void ImGuiApp::createImGuiDescriptorPool() {
  const std::vector<VkDescriptorPoolSize> pool_sizes = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  const uint32_t size                        = static_cast<uint32_t>(pool_sizes.size());
  const VkDescriptorPoolCreateInfo pool_info = {
      .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      .maxSets       = 1000 * size,
      .poolSizeCount = size,
      .pPoolSizes    = pool_sizes.data(),
  };

  if (vkCreateDescriptorPool(m_device.logical(), &pool_info, nullptr, &imGuiDescriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("Cannot allocate UI descriptor pool!");
  }
}
