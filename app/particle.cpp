// #include <particle/ParticleSystem.hpp>

#include <cxxopts.hpp>

int main(int argc, char** argv) {
  cxxopts::Options options(argv[0], "A program to load and visualize a 3D model !");

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("d,debug", "Debug level (0: nothing, 1: error, 2: warning)", cxxopts::value<int>(), "LEVEL")
    ("e,error-exit", "Exit on first error");
  ;
  // clang-format on

  auto result = options.parse(argc, argv);

  // need to init glfw first, to get the suitable glfw extension for the vkinstance
  // glfwInit();

  // // Disable OpenGL
  // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // int debugLevel = 0;
  // if (result.count("debug")) {
  //   debugLevel = result["debug"].as<int>();
  // }

  // vkl::DebugOption debugOption = {
  //     .debugLevel  = debugLevel,
  //     .exitOnError = result.count("error-exit") > 0,
  // };

  // vkl::ParticleSystem app(debugOption);

  // try {
  //   app.run();
  // } catch (std::exception& e) {
  //   std::cout << e.what() << std::endl;
  //   return EXIT_FAILURE;
  // }

  // glfwTerminate();

  return EXIT_SUCCESS;
}