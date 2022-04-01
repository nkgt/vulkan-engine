find_package(Vulkan REQUIRED COMPONENTS glslc)
message(STATUS "Found glslc: ${Vulkan_GLSLC_EXECUTABLE}")

function(compile_shaders)
    cmake_parse_arguments(arg "" "TARGET" "SOURCES" ${ARGN})

    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT  ${PROJECT_BINARY_DIR}/${source}.bin
            MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/${source}
            DEPFILE ${PROJECT_BINARY_DIR}/${source}.d
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} 
                    --target-env=vulkan1.3 -MD -MF ${PROJECT_BINARY_DIR}/${source}.d 
                    -o ${PROJECT_BINARY_DIR}/${source}.bin
                    ${PROJECT_SOURCE_DIR}/${source}
        )

        list(APPEND OUTPUTS ${source}.bin)
    endforeach()

    add_custom_target(${arg_TARGET}_shaders DEPENDS ${OUTPUTS})
    add_dependencies(${arg_TARGET} ${arg_TARGET}_shaders)
    
endfunction()
