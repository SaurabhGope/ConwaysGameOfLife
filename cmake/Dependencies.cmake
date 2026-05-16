include(FetchContent)

option(LIFE3D_FETCH_DEPENDENCIES "Fetch third-party dependencies during configure." ON)

if(NOT LIFE3D_FETCH_DEPENDENCIES)
    return()
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

if(TARGET glm::glm)
    set(LIFE3D_GLM_TARGET glm::glm)
elseif(TARGET glm)
    set(LIFE3D_GLM_TARGET glm)
endif()

set(LIFE3D_GLM_TARGET ${LIFE3D_GLM_TARGET} CACHE INTERNAL "Resolved GLM target name")

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.9
)
FetchContent_MakeAvailable(imgui)

if(NOT TARGET imgui)
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_compile_features(imgui PUBLIC cxx_std_20)
    target_link_libraries(imgui PUBLIC glfw)
endif()
