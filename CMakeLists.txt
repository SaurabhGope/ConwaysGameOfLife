cmake_minimum_required(VERSION 3.10)
project(ConwaysGameOfLife)

# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include current directory for headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# GLFW
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw)
include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib-vc2022)

# GLAD path setup
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
# Include GLAD
include_directories(${GLAD_DIR}/include)

# GLM path setup
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glm)
# Include GLM
include_directories(${GLM_DIR})

# Source files
add_executable(ConwaysGameOfLife
    main.cpp
    glad.c
    grid.cpp
    cell.cpp
    ansi_escapes.cpp
)

# Link GLFW and OpenGL (for Windows)
target_link_libraries(ConwaysGameOfLife glfw3 opengl32)

# Windows-specific settings (ANSI escape handling)
if(WIN32)
    target_compile_definitions(ConwaysGameOfLife PRIVATE _WIN32)
endif()