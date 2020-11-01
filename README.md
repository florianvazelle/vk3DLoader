# vk3DLoader [![Actions Status](https://github.com/florianvazelle/vk3DLoader/workflows/Windows/badge.svg)](https://github.com/florianvazelle/vk3DLoader/actions) [![Actions Status](https://github.com/florianvazelle/vk3DLoader/workflows/Ubuntu/badge.svg)](https://github.com/florianvazelle/vk3DLoader/actions)

This is work in progress Vulkan project to load and visualize a 3D model.

## Roadmap

- Implement :
  - Descriptor sets
  - Dynamic uniform buffers
  - Push constants
- 🏆 Support obj/fbx/gltf model loading

Bonus: Normal mapping

## Building

### Build and run the standalone target

Use the following command to build and run the executable target.

```bash
cmake -Bbuild
cmake --build build
./build/bin/vk3DLoader
```

### Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -Bbuild
cmake --build build
./build/bin/vk3DLoaderTests
```

## Dependencies

- C++17 compiler :
    - Visual Studio 2017 or 2019
    - GCC 7+ or Clang 5+
- [CMake](https://cmake.org/) for build system creation (>= 3.8)
- [Conan](https://conan.io/) for install packages (>= 1.0)