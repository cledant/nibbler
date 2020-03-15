cmake_minimum_required(VERSION 3.10)
project(nibbler)

set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/nibbler" CACHE STRING "" FORCE)

#Setup glm
add_library(glm INTERFACE IMPORTED)
set_target_properties(glm PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)

#Setup Glad
set(GLAD_API "gl=4.1")
option(GLAD_INSTALL "" ON)
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glad ${CMAKE_BINARY_DIR}/external/glad/build)

#Setup GLFW
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_INSTALL "" ON)
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_BINARY_DIR}/external/glfw/build)

#Setup SFML Window
option(SFML_BUILD_GRAPHICS "" FALSE)
option(SFML_BUILD_NETWORK "" FALSE)
option(SFML_BUILD_AUDIO "" FALSE)
option(SFML_BUILD_DOC "" FALSE)
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sfml ${CMAKE_BINARY_DIR}/sfml/build)

#Setup SDL2
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sdl2 ${CMAKE_BINARY_DIR}/sdl2/build)

#Setup freetype
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/freetype ${CMAKE_BINARY_DIR}/external/freetype/build)

#Setup portaudio
option(PA_BUILD_STATIC "" OFF)
option(PA_BUILD_SHARED "" ON)
OPTION(PA_ENABLE_DEBUG_OUTPUT "" OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/portaudio ${CMAKE_BINARY_DIR}/external/portaudio/build)

#gfx_dynamic libs
add_subdirectory(gfx_dynamic/libs)
add_subdirectory(gfx_dynamic/gl_wrapper)

#audio_dynamic libs
add_subdirectory(audio_dynamic/libs)
add_subdirectory(audio_dynamic/pa_wrapper)

#backend libs
add_subdirectory(backend/dyn_lib_loader)
add_subdirectory(backend/main_loop)

#Main binary
add_executable(nibbler binary/private/nibbler.cpp)
target_link_libraries(nibbler dl main_loop)
add_dependencies(nibbler gfx_dyn audio_dyn dyn_lib_loader gfx_dyn_glfw gfx_dyn_sfml gfx_dyn_sdl2 audio_dyn_portaudio main_loop)
target_compile_options(nibbler PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)

#Copy Shaders and Fonts
install(FILES
			${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/draw_rectangle/draw_rectangle_fs.glsl
			${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/draw_rectangle/draw_rectangle_gs.glsl
			${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/draw_rectangle/draw_rectangle_vs.glsl
		DESTINATION nibbler_shaders/draw_rectangle)
install(FILES
			${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/font/font_fs.glsl
			${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/font/font_vs.glsl
		DESTINATION nibbler_shaders/font)
install(FILES
			${CMAKE_CURRENT_SOURCE_DIR}/ressources/fonts/Roboto-Light.ttf
		DESTINATION nibbler_fonts)

#Copy nibbler binary
install(TARGETS nibbler
		RUNTIME DESTINATION .)