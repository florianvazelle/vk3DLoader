# vk3DLoader ![Platform](https://img.shields.io/badge/platform-windows%20%7C%20linux-brightgreen?style=flat-square) [![Actions Status](https://img.shields.io/github/workflow/status/florianvazelle/vk3DLoader/CMake%20Build%20Matrix?style=flat-square&color=brightgreen)](https://github.com/florianvazelle/vk3DLoader/actions) 

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
./build/bin/vk3DLoader --help
```

### Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -Bbuild
cmake --build build
./build/bin/vk3DLoaderTests
```

## Dependencies

- C++20 compiler :
    - Visual Studio 2019
    - GCC 9+ or Clang 10+
- [CMake](https://cmake.org/) for build system creation (>= 3.8)
- [Conan](https://conan.io/) for install packages (>= 1.0)

## References

- [Writing an Efficient Vulkan Renderer](https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/)
- [Vulkan Shader Resource-Binding](https://developer.nvidia.com/vulkan-shader-resource-binding)
- [Vulkan Input Attachments and Sub Passes](https://www.saschawillems.de/blog/2018/07/19/vulkan-input-attachments-and-sub-passes/)
- [Yet Another Blog Explaining Vulkan Synchronization](https://themaister.net/blog/2019/08/14/yet-another-blog-explaining-vulkan-synchronization/)
