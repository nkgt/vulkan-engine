cmake_minimum_required(VERSION 3.22)
project(vke_example_triangle VERSION 1.0.0)

add_executable(triangle src/triangle.cpp)

target_link_libraries(triangle PRIVATE vulkan_engine)

if(VKE_ASAN)
    target_compile_options(triangle PRIVATE ${vulkna_engine_ASANFLAGS})
    target_link_options(triangle PRIVATE ${vulkna_engine_ASANFLAGS})
endif()

target_compile_options(triangle
    PRIVATE $<$<CONFIG:Debug>:${vulkan_engine_DBGFLAGS}>
)