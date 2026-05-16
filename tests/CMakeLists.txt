add_executable(life3d_smoke
    smoke/SmokeTest.cpp
)

target_link_libraries(life3d_smoke PRIVATE life3d_sim life3d_seed life3d_render)
target_compile_features(life3d_smoke PRIVATE cxx_std_20)
target_include_directories(life3d_smoke PRIVATE ${PROJECT_SOURCE_DIR}/include)
life3d_apply_project_warnings(life3d_smoke)

add_test(NAME life3d_smoke COMMAND life3d_smoke)

add_custom_target(life3d_tests DEPENDS life3d_smoke)
