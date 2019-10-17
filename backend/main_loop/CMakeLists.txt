cmake_minimum_required(VERSION 3.10)
project(binary_main_loop)

add_library(main_loop STATIC
		private/World.cpp)
target_include_directories(main_loop
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(main_loop gfx_dyn dyn_lib_loader)
target_link_libraries(main_loop PUBLIC gfx_dyn dyn_lib_loader)
target_compile_options(main_loop PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)
