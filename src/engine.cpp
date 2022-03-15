#include "nkgt/engine.hpp"

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

namespace nkgt {

void glfw_error_callback(int error_code, const char* error_description) {
    spdlog::error("GLFW - {} (Code {})", error_description, error_code);
}

Engine::Engine(std::string name, int width, int height)
    : window_{ .name = std::move(name),
               .width = width,
               .height = height,
               .handle = nullptr } {
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window_.handle = glfwCreateWindow(
        window_.width, 
        window_.height,
        window_.name.c_str(), 
        nullptr, 
        nullptr
    );
}

Engine::~Engine() {
    glfwTerminate();
}

void Engine::run() const noexcept {
    while(!glfwWindowShouldClose(window_.handle)) {
        glfwPollEvents();
    }
} 

} // namespace nkgt