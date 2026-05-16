cmake_minimum_required(VERSION 3.24)

project(Life3DEngine VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerOptions.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

set(LIFE3D_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(life3d_configure_target target_name)
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_include_directories(${target_name} PUBLIC ${LIFE3D_INCLUDE_DIR})

    if(DEFINED LIFE3D_GLM_TARGET AND TARGET ${LIFE3D_GLM_TARGET})
        target_link_libraries(${target_name} PUBLIC ${LIFE3D_GLM_TARGET})
    endif()

    life3d_apply_project_warnings(${target_name})
endfunction()

find_package(OpenGL REQUIRED)
find_package(Vulkan QUIET)

if(Vulkan_FOUND)
    message(STATUS "Vulkan SDK found: ${Vulkan_VERSION}")
else()
    message(STATUS "Vulkan SDK not found. Vulkan backend will remain unavailable in this build.")
endif()

add_library(life3d_sim
    src/sim/RuleSet.cpp
    src/sim/Grid3D.cpp
    src/sim/SimulationEngine.cpp
)
life3d_configure_target(life3d_sim)

add_library(life3d_seed
    src/seed/SeedEngine.cpp
)
life3d_configure_target(life3d_seed)
target_link_libraries(life3d_seed PUBLIC life3d_sim)

add_library(life3d_render
    src/render/RendererFactory.cpp
    src/render/opengl/OpenGLRenderer.cpp
    src/render/opengl/OpenGLShaders.cpp
)
life3d_configure_target(life3d_render)
target_link_libraries(life3d_render PUBLIC glfw OpenGL::GL)
if(Vulkan_FOUND)
    target_compile_definitions(life3d_render PUBLIC LIFE3D_HAS_VULKAN=1)
    target_link_libraries(life3d_render PUBLIC Vulkan::Vulkan)
endif()

add_library(life3d_ui
    src/ui/UiLayer.cpp
)
life3d_configure_target(life3d_ui)
if(TARGET imgui)
    target_link_libraries(life3d_ui PUBLIC imgui)
endif()

add_library(life3d_core
    src/core/Application.cpp
    src/camera/Camera.cpp
    src/platform/Window.cpp
)
life3d_configure_target(life3d_core)
target_link_libraries(life3d_core
    PUBLIC
        life3d_sim
        life3d_seed
        life3d_render
        life3d_ui
        glfw
        OpenGL::GL
)

add_executable(life3d_app
    src/main.cpp
)
life3d_configure_target(life3d_app)
target_link_libraries(life3d_app PRIVATE life3d_core)
set_target_properties(life3d_app PROPERTIES OUTPUT_NAME life3d)

add_custom_command(TARGET life3d_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:life3d_app>/shaders
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS life3d_app RUNTIME DESTINATION .)
install(DIRECTORY shaders DESTINATION .)
install(FILES README.md DESTINATION .)

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME "Life3D")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "Life3D-${PROJECT_VERSION}-windows-x64")
include(CPack)
