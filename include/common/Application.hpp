/**
 * @file Application.hpp
 * @brief Define Application base class
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <NonCopyable.hpp>
#include <common/DebugUtilsMessenger.hpp>
#include <common/Device.hpp>
#include <common/Instance.hpp>
#include <common/SwapChain.hpp>
#include <common/SyncObjects.hpp>
#include <common/Window.hpp>

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