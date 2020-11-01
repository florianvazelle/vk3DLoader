#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class ShaderLoader {
public:
  static std::vector<char> load(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("Unable to open SPIR-V file: " + filePath
                               + "\nDid you run compile.sh yet?");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
  }

private:
  ShaderLoader() = default;
};