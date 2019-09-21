cmake_minimum_required(VERSION 3.10)
project(gfx_dynamic_libs_libs)

add_library(gfx_dyn INTERFACE)
target_include_directories(gfx_dyn INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/public)

add_library(gfx_dyn_glfw SHARED
		private/GlfwGraphic.cpp)
target_include_directories(gfx_dyn_glfw
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_dyn_glfw gfx_dyn)
target_link_libraries(gfx_dyn_glfw gfx_dyn)
target_compile_options(gfx_dyn_glfw PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)