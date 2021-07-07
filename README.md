[![Actions Status](https://github.com/florianvazelle/vk3DLoader/workflows/build/badge.svg)](https://github.com/florianvazelle/vk3DLoader/actions)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/fdf05c587b994f008692d4e6fd1f0acd)](https://www.codacy.com/gh/florianvazelle/vk3DLoader/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=florianvazelle/vk3DLoader&amp;utm_campaign=Badge_Grade)
![Platform](https://img.shields.io/badge/platform-windows%20%7C%20linux-blue) 
[![License](https://img.shields.io/badge/license-Unlicense-blue.svg)](https://github.com/florianvazelle/vk3DLoader/blob/main/LICENSE)

# vk3DLoader

This is a work in progress Vulkan project to load and visualize a 3D model.

## Building

### Build and run the standalone target

Use the following command to build and run the executable target.

```bash
cmake -Bbuild
cmake --build build
./build/bin/vk3DLoader --help
```

## Dependencies

- C++20 compiler :
  - Visual Studio 2019
  - GCC 9+ or Clang 10+
- [CMake](https://cmake.org/) for build system creation (>= 3.16.3)
- [Conan](https://conan.io/) for install packages (>= 1.0)

## Quote to save many hours

> Input attachments of course are not "textures"; they're attachments.