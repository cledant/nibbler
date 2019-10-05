cmake_minimum_required(VERSION 3.10)
project(gfx_dynamic_lib)

add_subdirectory(libs)
add_subdirectory(loader)
add_subdirectory(gl_wrapper)

add_executable(test_gfx
		test_binary_libs/private/test_binary.cpp)
add_dependencies(test_gfx glad glfw gfx_dyn gfx_dyn_glfw gfx_loader glwrapper)
target_link_libraries(test_gfx PUBLIC glad glfw glm gfx_loader gfx_dyn dl glwrapper)
target_compile_options(test_gfx PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)
set_target_properties(test_gfx
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")