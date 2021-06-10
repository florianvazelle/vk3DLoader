// clang-format off
#include <common/Application.hpp>
#include <common/DebugUtilsMessenger.hpp>  // for vkl
#include <common/Instance.hpp>             // for Instance, Instance::Device...
#include <common/SwapChain.hpp>            // for SwapChain
#include <glm/glm.hpp>
// clang-format on

using namespace vkl;

Application::Application(const std::string& appName, const DebugOption& debugOption)
    : instance(appName, ENGINE_NAME, (debugOption.debugLevel > 0)),
      debugMessenger(instance, debugOption.exitOnError),
      window({WIDTH, HEIGHT}, appName, instance),
      device(instance, window, Instance::DeviceExtensions),
      swapChain(device, window),
      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT) {}
