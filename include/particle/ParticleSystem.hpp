/**
 * @file ParticleSystem.hpp
 * @brief Define ParticleSystem class
 *
 * This is our Vulkan App, call by the main and run the main loop.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <common/Application.hpp>

namespace vkl {
  class ParticleSystem : public Application{
  public:
    ParticleSystem(const std::string& appName, const DebugOption& debugOption);

    void run(std::function<void(void)> update);

  private:
    size_t currentFrame = 0;

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized);
  };

}  // namespace vkl