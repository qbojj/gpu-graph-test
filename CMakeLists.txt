cmake_minimum_required(VERSION 3.8)

project(gpu-graph-test)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm REQUIRED)

add_executable(gpu-graph-test
    gpu-graph-test.cpp
    glad/src/gl.c
)
target_include_directories(gpu-graph-test PRIVATE
    glad/include
)

target_link_libraries(gpu-graph-test
    glfw
    glm::glm
)