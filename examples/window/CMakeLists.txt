cmake_minimum_required(VERSION 3.22)
project(vke_example_window VERSION 1.0.0)

add_executable(window src/window.cpp)

target_link_libraries(window PRIVATE glfw)

if(VKE_ASAN)
    target_compile_options(window PRIVATE ${vulkna_engine_ASANFLAGS})
    target_link_options(window PRIVATE ${vulkna_engine_ASANFLAGS})
endif()

target_compile_options(window
    PRIVATE $<$<CONFIG:Debug>:${vulkan_engine_DBGFLAGS}>
)