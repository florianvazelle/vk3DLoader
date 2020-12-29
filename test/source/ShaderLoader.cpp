#include <doctest/doctest.h>

#include <vkl/ShaderLoader.hpp>

// Obvious test, you can remove
TEST_CASE("Load Shader") {
  try {
    auto vertShaderCode = ShaderLoader::load(DATA_PATH "/shader/base.vert.spv");
    CHECK(vertShaderCode.size() > 0);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    CHECK(false);
  }
}
