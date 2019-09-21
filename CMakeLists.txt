cmake_minimum_required(VERSION 3.10)
project(nibbler)

#Setup Glad
set(GLAD_API "gl=4.1")
option(GLAD_INSTALL "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glad ${CMAKE_BINARY_DIR}/external/glad/build)

#Setup GLFW
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_INSTALL "" ON)
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_BINARY_DIR}/external/glfw/build)

add_subdirectory(gfx_dynamic_libs)

add_executable(nibbler binary/private/nibbler.cpp)
add_dependencies(nibbler gfx_dyn gfx_dyn_glfw test_gfx)
target_compile_options(nibbler PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)