#include <vkl/Application.hpp>

#include <cxxopts.hpp>

int main(int argc, char** argv) {
  cxxopts::Options options(argv[0], "A program to load and visualize a 3D model !");
  options.positional_help("FILE").show_positional_help();

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("m,model", "Path to a model to visualize (if not specified draw a triangle)", cxxopts::value<std::string>(), "FILE");
  options.add_options("Dev")
    ("d,debug", "Debug level (0: nothing, 1: error, 2: warning)", cxxopts::value<int>(), "LEVEL")
    ("e,error-exit", "Exit on first error");
  ;
  // clang-format on

  options.parse_positional({"model"});

  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help();
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

  std::string modelPath = "";
  if (result.count("model")) {
    modelPath = result["model"].as<std::string>();
  } else {
    std::cout << "You must specify a model to load.\n"
              << "Use « vk3DLoader --help » for more information.\n";
    return 0;
  }

  int debugLevel = 0;
  if (result.count("debug")) {
    debugLevel = result["debug"].as<int>();
  }

  vkl::DebugOption debugOption = {
      .debugLevel  = debugLevel,
      .exitOnError = result.count("error-exit") > 0,
  };

  vkl::Application app(debugOption, modelPath);

  try {
    app.run();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}