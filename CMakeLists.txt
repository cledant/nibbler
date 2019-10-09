cmake_minimum_required(VERSION 3.10)
project(nibbler)

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

#gfx_dynamic libs
add_subdirectory(gfx_dynamic/libs)
add_subdirectory(gfx_dynamic/loader)
add_subdirectory(gfx_dynamic/gl_wrapper)

#backend libs
add_subdirectory(backend/main_loop)

#Main binary
add_executable(nibbler binary/private/nibbler.cpp)
target_link_libraries(nibbler main_loop gfx_dyn_glfw dl)
add_dependencies(nibbler gfx_dyn gfx_dyn_glfw main_loop)
target_compile_options(nibbler PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)

#test_binary
add_executable(test_gfx
		test_binary_libs/private/test_binary.cpp)
add_dependencies(test_gfx glad glfw gfx_dyn gfx_dyn_glfw gfx_loader glwrapper)
target_link_libraries(test_gfx PUBLIC glad glfw glm gfx_loader gfx_dyn dl glwrapper)
target_compile_options(test_gfx PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native -flto)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/draw_rectangle DESTINATION ${CMAKE_INSTALL_PREFIX}/nibbler_shaders)