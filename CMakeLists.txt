cmake_minimum_required(VERSION 3.22)
project(vulkan-engine VERSION 0.0.1)

find_package(fmt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(vulkan_engine_SOURCES
    src/window.cpp
)

set(vulkan_engine_HEADERS
    src/window.hpp
)

# Header files are explicitely added to the target so that they show up in IDEs
# like Visual Studio as part of the project file structure.
add_library(vulkan_engine STATIC
    ${vulkan_engine_SOURCES}
    # This should probably be generator based (i.e. Ninja, Makefiles, VS, etc.)
    # instead of platform based.
    $<$<PLATFORM_ID:Windows>:${vulkan_engine_HEADERS}>
)

target_include_directories(vulkan_engine PUBLIC include)
target_link_libraries(vulkan_engine
    PRIVATE fmt::fmt
    PRIVATE glfw
    PRIVATE glm::glm
    PRIVATE spdlog::spdlog
)

set(vulkan_engine_DBGFLAGS
    $<$<CXX_COMPILER_ID:Clang,GNU>:
        -Og
        -Wall 
        -Wextra
        -Wshadow 
        -Wnon-virtual-dtor 
        -pedantic
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wnull-dereference
        -Wuseless-cast
        -Wdouble-promotion
        -Wformat=2
        -Wlifetime
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /permissive-
        /W4
        /w14242
        /w14254
        /w14263
        /w14265
        /w14287
        /we4289
        /w14296
        /w14311
        /w14545
        /w14546
        /w14547
        /w14549
        /w14555
        /w14619
        /w14640
        /w14826
        /w14905
        /w14906
        /w14928
    >
)

target_compile_options(vulkan_engine
    PRIVATE $<$<CONFIG:Debug>:${vulkan_engine_DBGFLAGS}>
)

set_target_properties(vulkan_engine PROPERTIES CXX_STANDARD 20)