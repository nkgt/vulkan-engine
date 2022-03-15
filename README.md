# Vulkan Engine
This repository contains a simple 3D rendering engine based on [Vulkan](https://www.vulkan.org/) I am developing in my free time as a passion project.
The main reason I am developing this engine is for exploration of graphics API and graphics techniques.
This engine tries to support both Windows with Visual Studio and Linux with both gcc and Clang.

## Build
The project uses [CMake](https://cmake.org/) for its configuration and depends on the following external libraries on top of Vulkan itself

- [fmt](https://github.com/fmtlib/fmt)
- [glfw](https://www.glfw.org/)
- [glm](https://github.com/g-truc/glm)
- [spdlog](https://github.com/gabime/spdlog)

During the actual configuration steps in the CMake script, these libraries are directly searched in your system through `find_package()` commands. In case you don't have these libraries installed globally in your system the repository also includes a manifest for [vcpkg](https://vcpkg.io/en/index.html) which can download these dependencies for you and nicely integrates with CMake as explained [here](https://vcpkg.io/en/docs/examples/manifest-mode-cmake.html).

Regardless of how you have obtained the dependencies, the project can be built as follows on Linux from the root of the repository
```sh
> mkdir build
> cd build
> cmake ..
> make -j
```
Similarly, on Windows you can substitute the last step with opening the generated Visual Studio solution and then compiling from whithin the IDE.

### CMake variables
The project's configuration can be changed through the following CMake variables

- `VKE_ASAN` activates sanitizers in gcc and Clang through the flags `-fsanitize=address,pointer-compare,pointer-subtract,leak,undefined -fsanitize-address-use-after-scope`. Note that these flags are set to PUBLIC in CMake and as a consequence they will be active for all CMake targets linking the library
- `VKE_BUILD_EXAMPLES` activates the compilation of the example executables in the `example` subdirectory