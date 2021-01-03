#include <vkl/Application.hpp>

#include <cxxopts.hpp>

int main(int argc, char** argv) {
  cxxopts::Options options(argv[0], "A program to load and visualize a 3D model !");

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("m,model", "Path to a model to visualize", cxxopts::value<std::string>()->default_value(""))
    ("t,texture", "Path to the texture attached to the model", cxxopts::value<std::string>()->default_value(""))
  ;
  // clang-format on

  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help() << std::endl;
    return 0;
  } else if (result["version"].as<bool>()) {
    std::cout << "vk3DLoader, version " << VK3DLOADER_VERSION << std::endl;
    return 0;
  }

  // need to init glfw first, to get the suitable glfw extension for the vkinstance
  glfwInit();

  // Disable OpenGL
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  vkl::Application app(result["model"].as<std::string>(), result["texture"].as<std::string>());

  try {
    app.run();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}