cmake_minimum_required(VERSION 3.10)
project(gfx_dynamic_libs_libs)

add_library(gfx_dyn INTERFACE)
target_include_directories(gfx_dyn INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/public)

#LIB GLFW
add_library(gfx_dyn_glfw SHARED
		private/GlfwGraphic.cpp)
target_include_directories(gfx_dyn_glfw
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_dyn_glfw gfx_dyn)
target_link_libraries(gfx_dyn_glfw PUBLIC gfx_dyn)
target_compile_options(gfx_dyn_glfw PUBLIC -Wall -Wextra -Werror -std=c++17 -march=native)

#LIB SFML
add_library(gfx_dyn_sfml SHARED
		private/SFMLGraphic.cpp)
target_include_directories(gfx_dyn_sfml
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_dyn_sfml gfx_dyn)
target_link_libraries(gfx_dyn_sfml PUBLIC gfx_dyn)
target_compile_options(gfx_dyn_sfml PUBLIC -Wall -Wextra -Werror -std=c++17 -march=native)

set_target_properties(gfx_dyn_glfw gfx_dyn_sfml
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dyn_libs")