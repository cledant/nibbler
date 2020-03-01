cmake_minimum_required(VERSION 3.10)
project(pa_wrapper_libs)

#LIB PortAudio
add_library(pa_wrapper SHARED
		private/PaSound.cpp)
target_include_directories(pa_wrapper
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public)
add_dependencies(pa_wrapper portaudio)
target_link_libraries(pa_wrapper PUBLIC portaudio)
target_compile_options(pa_wrapper PUBLIC -Wall -Wextra -Werror -std=c++17 -march=native)

set_target_properties(pa_wrapper
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/nibbler_libs
		SKIP_BUILD_RPATH FALSE
		BUILD_WITH_INSTALL_RPATH TRUE
		INSTALL_RPATH_USE_LINK_PATH TRUE
		INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib64"
		INSTALL_RPATH_USE_LINK_PATH TRUE)