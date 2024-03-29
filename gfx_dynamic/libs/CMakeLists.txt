cmake_minimum_required(VERSION 3.10)
project(gfx_dynamic_libs_libs)

add_library(gfx_dyn INTERFACE)
target_include_directories(gfx_dyn INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/public)
target_link_libraries(gfx_dyn INTERFACE glm)

#LIB GLFW
add_library(gfx_dyn_glfw SHARED
		private/GlfwGraphic.cpp)
target_include_directories(gfx_dyn_glfw
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_dyn_glfw glad glfw gfx_dyn glwrapper)
target_link_libraries(gfx_dyn_glfw PUBLIC glad glfw glm gfx_dyn glwrapper)
target_compile_options(gfx_dyn_glfw PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)

#LIB SFML
add_library(gfx_dyn_sfml SHARED
		private/SFMLGraphic.cpp)
target_include_directories(gfx_dyn_sfml
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_dyn_sfml glad sfml-window sfml-system gfx_dyn glwrapper)
target_link_libraries(gfx_dyn_sfml PUBLIC glad sfml-window sfml-system glm gfx_dyn glwrapper)
target_compile_options(gfx_dyn_sfml PUBLIC -Wall -Wextra -Werror -std=c++17 -march=native)

#Lib SDL2
add_library(gfx_dyn_sdl2 SHARED
		private/SDL2Graphic.cpp)
target_include_directories(gfx_dyn_sdl2
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(gfx_dyn_sdl2 glad SDL2 gfx_dyn glwrapper)
target_link_libraries(gfx_dyn_sdl2 PUBLIC glad SDL2 glm gfx_dyn glwrapper)
target_compile_options(gfx_dyn_sdl2 PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)

set_target_properties(gfx_dyn_glfw gfx_dyn_sfml gfx_dyn_sdl2
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/nibbler_libs
		SKIP_BUILD_RPATH FALSE
		BUILD_WITH_INSTALL_RPATH TRUE
		INSTALL_RPATH_USE_LINK_PATH TRUE
		INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib64"
		INSTALL_RPATH_USE_LINK_PATH TRUE)