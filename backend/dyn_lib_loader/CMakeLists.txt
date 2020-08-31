cmake_minimum_required(VERSION 3.10)
project(dyn_lib_loader_nibbler)

add_library(dyn_lib_loader INTERFACE)
target_include_directories(dyn_lib_loader
		INTERFACE
		public)
target_compile_options(dyn_lib_loader INTERFACE -Wall -Wextra -Werror -std=c++17 -march=native)