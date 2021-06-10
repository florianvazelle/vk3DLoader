/**
 * @file Application.hpp
 * @brief Define Application base class
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

// clang-format off
#include <NonCopyable.hpp>                 // for NonCopyable
#include <common/DebugUtilsMessenger.hpp>  // for DebugUtilsMessenger
#include <common/Device.hpp>               // for Device
#include <common/Instance.hpp>             // for Instance
#include <common/SwapChain.hpp>            // for SwapChain
#include <common/SyncObjects.hpp>          // for SyncObjects
#include <common/Window.hpp>               // for Window
#include <string>                          // for allocator, string
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

  class Application : public NonCopyable {
  public:
    Application(const std::string& appName, const DebugOption& debugOption);

  protected:
    Instance instance;
    DebugUtilsMessenger debugMessenger;
    Window window;
    Device device;
    SwapChain swapChain;
    SyncObjects syncObjects;
  };
}  // namespace vkl

#endif  // APPLICATION_HPP