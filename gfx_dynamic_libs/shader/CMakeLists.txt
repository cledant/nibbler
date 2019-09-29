cmake_minimum_required(VERSION 3.10)
project(shader_lib)

add_library(shader STATIC
		private/Shader.cpp)
target_include_directories(shader
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(shader glad)
target_link_libraries(shader PUBLIC glad)
target_compile_options(shader PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)