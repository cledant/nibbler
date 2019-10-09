cmake_minimum_required(VERSION 3.10)
project(gl_wrapper_lib)

add_library(glwrapper STATIC
		private/GLShader.cpp private/GLSnake.cpp private/GLBoard.cpp)
target_include_directories(glwrapper
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(glwrapper glad)
target_link_libraries(glwrapper PUBLIC glad glm gfx_dyn)
target_compile_options(glwrapper PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)