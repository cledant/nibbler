cmake_minimum_required(VERSION 3.10)
project(audio_dynamic_libs_libs)

add_library(audio_dyn INTERFACE)
target_include_directories(audio_dyn INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/public)

#LIB PortAudio
add_library(audio_dyn_portaudio SHARED
		private/PortAudio.cpp)
target_include_directories(audio_dyn_portaudio
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(audio_dyn_portaudio portaudio pa_wrapper audio_dyn)
target_link_libraries(audio_dyn_portaudio PUBLIC portaudio pa_wrapper audio_dyn)
target_compile_options(audio_dyn_portaudio PUBLIC -Wall -Wextra -Werror -std=c++17 -march=native)

set_target_properties(audio_dyn_portaudio
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/nibbler_libs
		SKIP_BUILD_RPATH FALSE
		BUILD_WITH_INSTALL_RPATH TRUE
		INSTALL_RPATH_USE_LINK_PATH TRUE
		INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib64"
		INSTALL_RPATH_USE_LINK_PATH TRUE)