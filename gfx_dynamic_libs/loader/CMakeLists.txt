cmake_minimum_required(VERSION 3.10)
project(gfx_loader_lib_nibbler)

add_library(gfx_loader SHARED
		private/gfx_loader.cpp)
target_include_directories(gfx_loader
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_loader gfx_dyn)
target_compile_options(gfx_loader PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)