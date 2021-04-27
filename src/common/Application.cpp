#include <common/Application.hpp>

using namespace vkl;

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::string ENGINE_NAME = "No Engine";

Application::Application(const std::string& appName, const DebugOption& debugOption)
    : instance(appName, ENGINE_NAME, (debugOption.debugLevel > 0)),
      debugMessenger(instance, debugOption.exitOnError),
      window({WIDTH, HEIGHT}, appName, instance),
      device(instance, window, Instance::DeviceExtensions),
      swapChain(device, window),
      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT) {}
