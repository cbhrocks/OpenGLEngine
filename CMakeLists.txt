cmake_minimum_required(VERSION 3.13)
project (VideoGame)

add_executable(VideoGame src/main.cpp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../../Programs/glfw-3.2.1/ lib/glfw)
add_subdirectory(../../Programs/glad/ lib/glad)
add_subdirectory(../../Programs/glm/ lib/glm)
add_subdirectory(../../Programs/tinyobjloader/ lib/tinyobjloader)

target_link_libraries(VideoGame glfw glad glm tinyobjloader)

find_package(OpenGL REQUIRED)

target_include_directories(VideoGame PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(VideoGame ${OPENGL_gl_LIBRARY})
