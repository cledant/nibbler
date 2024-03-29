cmake_minimum_required(VERSION 3.10)
project(binary_main_loop)

add_library(main_loop STATIC
		private/World.cpp
		private/World_events.cpp
		private/Board.cpp
		private/Players.cpp
		private/Snake.cpp)
target_include_directories(main_loop
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private
		${CMAKE_CURRENT_BINARY_DIR}/private)

configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/private/lib_path.hpp.in"
		"${CMAKE_CURRENT_BINARY_DIR}/private/lib_path.hpp"
)

add_dependencies(main_loop gfx_dyn audio_dyn dyn_lib_loader)
target_link_libraries(main_loop PUBLIC gfx_dyn audio_dyn dyn_lib_loader)
target_compile_options(main_loop PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)
