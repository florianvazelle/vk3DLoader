/**
 * @file Application.hpp
 * @brief Define Application base class
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

// clang-format off
#include <NoCopy.hpp>                 // for NoCopy
#include <common/DebugUtilsMessenger.hpp>  // for DebugUtilsMessenger
#include <common/Device.hpp>               // for Device
#include <common/Instance.hpp>             // for Instance
#include <common/SwapChain.hpp>            // for SwapChain
#include <common/SyncObjects.hpp>          // for SyncObjects
#include <common/Window.hpp>               // for Window
#include <string>                          // for allocator, string
#include <iostream>
// clang-format on

#define MAX_FRAMES_IN_FLIGHT 2

#define WIDTH 800
#define HEIGHT 600

#define ENGINE_NAME "No Engine"

/**
 * @brief Vulkan Loader
 *
 * Encapsulates the entire vk3DLoader library.
 */
namespace vkl {

  struct DebugOption {
    int debugLevel;
    bool exitOnError;
  };

  class Application : public NoCopy {
  public:
    Application(const std::string& appName, const DebugOption& debugOption);

  protected:
    Instance instance;
    DebugUtilsMessenger debugMessenger;
    Window window;
    Device device;
    SwapChain swapChain;
    SyncObjects syncObjects;

    size_t currentFrame = 0;

    VkResult prepareFrame(bool& framebufferResized, uint32_t& imageIndex);
    void submitFrame(bool& framebufferResized, const uint32_t& imageIndex);

    virtual void recreateSwapChain(bool& framebufferResized) {
      std::cout << "warning: call base recreateSwapChain" << std::endl;
    };
  };
}  // namespace vkl

#endif  // APPLICATION_HPP