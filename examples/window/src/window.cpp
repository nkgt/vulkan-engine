#include "GLFW/glfw3.h"

#include <cstdlib>
#include <memory>

// This example leaks 1688B of memory on Ubuntu 18.04 as per Asan. 
// According to https://github.com/glfw/glfw/issues/1195 this is due to libX11
// 2:1.6.4-3 (and a nVidia GPU?) and is solved by libX11 2:1.6.8-1 which is 
// available in more recent versions of Ubuntu.
int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(800, 600, "Vulkan Engine Example", nullptr, nullptr);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Automatically handles the remaining GLFWWindow pointers.
    glfwTerminate();

    return EXIT_SUCCESS;
}