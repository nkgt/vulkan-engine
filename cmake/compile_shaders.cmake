find_package(Vulkan REQUIRED COMPONENTS glslc)
message(STATUS "Found glslc: ${Vulkan_GLSLC_EXECUTABLE}")

function(compile_shaders target)
    cmake_parse_arguments(PARSER_ARGV 1 arg "" "" "SOURCES")

    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/shaders)

    message(STATUS ${arg_SOURCES})
    message(STATUS ${arg_UNPARSED_ARGUMENTS})
    message(STATUS ${arg_KEYWORDS_MISSING_VALUES})

    foreach(source ${arg_SOURCES})
        message(STATUS "Shader: ${source}")
        add_custom_command(
            OUTPUT  ${source}.bin
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND ${Vulkan_GLSLC_EXECUTABLE}
                    --target-env=vulkan1.3 -MD -MF ${source}.d
                    -o ${PROJECT_BINARY_DIR}/shaders/${source}.bin
                    ${source}
        )
        # Why?
        target_sources(${target} PRIVATE ${PROJECT_BINARY_DIR}/shaders/${source}.bin)
    endforeach()
    
endfunction()
