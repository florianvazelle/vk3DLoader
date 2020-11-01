cmake_minimum_required(VERSION 3.8)

# ---- Project ----

project(vk3DLoader VERSION 1.2.3.4 LANGUAGES CXX)
set(LIBRARY_NAME vkStarter)
set(PROJECT_CXX_STANDARD 17)

include(FindVulkan)
if(NOT VULKAN_FOUND)
  message(FATAL_ERROR "Vulkan SDK not installed.")
endif()

# ---- Options ----

option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." OFF)
option(ENABLE_TESTING "Whether to generate unit test program and test" ON)

# ---- Add source files ----
file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/source/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE PROJECT_ALL_FILES "include/*.hpp" "source/*.cpp" "app/*.cpp" "test/*.cpp")

# ---- Tool ----
file(GLOB_RECURSE PROJECT_MODULES "${CMAKE_SOURCE_DIR}/cmake/*.cmake")
foreach(ARG ${PROJECT_MODULES})
  include(${ARG})
endforeach()

# ---- Conan package ----
if(CONAN_EXPORTED)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    include("${CMAKE_SOURCE_DIR}/cmake/manual/conan.cmake")
    conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP BUILD missing)
endif()

file(GLOB_RECURSE EXTERNALS "${CMAKE_CURRENT_BINARY_DIR}/external/*")

# ---- Shaders ----
file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/data/shader/*.vert" "${CMAKE_SOURCE_DIR}/data/shader/*.frag")
list(APPEND COMPILED_SHADERS "")
foreach (SHADER ${SHADERS})
  list(APPEND COMPILED_SHADERS "${SHADER}.spv")
endforeach()

# ---- Create library ----
add_library(${LIBRARY_NAME} OBJECT ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${EXTERNALS} ${SHADERS} ${COMPILED_SHADERS})

# Lib needs its header files, and users of the library must also see these (PUBLIC).
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/external)

target_link_libraries(${LIBRARY_NAME} PUBLIC ${CONAN_LIBS} Vulkan::Vulkan)
target_compile_definitions(${LIBRARY_NAME} PUBLIC IMGUI_IMPL_VULKAN)

target_set_warnings(${LIBRARY_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)

# ---- Create Entry Point ----

# Add an executable for the file app/main.cc.
# If you add more executables, copy these lines accordingly.
add_executable(${PROJECT_NAME} app/main.cpp)

# Link the executable to library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARY_NAME})

target_set_warnings(${PROJECT_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)

# Set the properties you require, e.g. what C++ standard to use. Here applied to library and main.
set_target_properties(
      ${PROJECT_NAME} ${LIBRARY_NAME}
      PROPERTIES 
        CXX_STANDARD ${PROJECT_CXX_STANDARD} 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)

add_compile_definitions(DATA_PATH="${PROJECT_SOURCE_DIR}/data")

# Set up tests (see test/CMakeLists.txt).
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# ---- Compile shader into SPIR-V ----
if (WIN32)
  set(glslCompiler "glslangValidator.exe")
else()
  set(glslCompiler "glslangValidator")
endif()

foreach(SHADER ${SHADERS})
  add_custom_command(
    OUTPUT ${SHADER}.spv
    COMMAND ${PROJECT_BINARY_DIR}/bin/${glslCompiler} -V ${SHADER} -o ${SHADER}.spv
    DEPENDS ${SHADER}
    COMMENT "Rebuilding ${SHADER}.spv"
  )
  message(STATUS "Generating build commands for ${SHADER}.spv")
endforeach()