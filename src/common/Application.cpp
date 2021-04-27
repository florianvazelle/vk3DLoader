#include <common/Application.hpp>

using namespace vkl;

Application::Application(const std::string& appName, const DebugOption& debugOption)
    : instance(appName, ENGINE_NAME, (debugOption.debugLevel > 0)),
      debugMessenger(instance, debugOption.exitOnError),
      window({WIDTH, HEIGHT}, appName, instance),
      device(instance, window, Instance::DeviceExtensions),
      swapChain(device, window),
      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT) {}
