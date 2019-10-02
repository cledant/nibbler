cmake_minimum_required(VERSION 3.10)
project(nibbler)

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

#Setup glm
#option(BUILD_SHARED_LIBS "" OFF)
#option(BUILD_STATIC_LIBS "" OFF)
option(GLM_TEST_ENABLE OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glm ${CMAKE_BINARY_DIR}/external/glm/build)

add_subdirectory(gfx_dynamic_libs)

add_executable(nibbler binary/private/nibbler.cpp)
add_dependencies(nibbler glfw glad glm gfx_dyn gfx_dyn_glfw test_gfx)
target_compile_options(nibbler PRIVATE -Wall -Wextra -Werror -std=c++17 -march=native)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ressources/shaders/draw_rectangle DESTINATION ${CMAKE_INSTALL_PREFIX}/nibbler_shaders)