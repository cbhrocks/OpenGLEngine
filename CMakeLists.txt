cmake_minimum_required(VERSION 3.13)
project (VideoGame)

add_executable(VideoGame src/main.cpp)

set(GCC_DEBUG_COMPILE_FLAGS "-g -fstack-protector-all")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../../Libraries/glad/ lib/glad)
add_subdirectory(../../Libraries/glfw/ lib/glfw)
add_subdirectory(../../Libraries/glm/ lib/glm)
add_subdirectory(../../Libraries/assimp/ lib/assimp)

add_definitions(${GCC_DEBUG_COMPILE_FLAGS})

target_link_libraries(VideoGame glad glfw glm assimp)

find_package(OpenGL REQUIRED)

target_include_directories(VideoGame PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(VideoGame ${OPENGL_gl_LIBRARY})
