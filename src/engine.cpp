#include "nkgt/engine.hpp"

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include <cstdint>

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

    vk::ApplicationInfo app_info(
        window_.name.c_str(), 
        VK_MAKE_VERSION(0, 0, 1),
        "VulkanToy",
        VK_MAKE_VERSION(0, 0, 1),
        VK_API_VERSION_1_3
    );

    uint32_t glfw_extension_count;
    const char* const* glfw_extensions = glfwGetRequiredInstanceExtensions(
        &glfw_extension_count
    );

    std::vector<const char*> requested_extensions(
        glfw_extensions,
        glfw_extensions + glfw_extension_count
    );
#ifndef NDEBUG
    requested_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef NDEBUG
    std::vector<const char*> requested_layers = {};
#else
    std::vector<const char*> requested_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
#endif

    vk::InstanceCreateInfo create_info(
        {}, 
        &app_info, 
        static_cast<uint32_t>(requested_layers.size()), 
        requested_layers.data(), 
        static_cast<uint32_t>(requested_extensions.size()), 
        requested_extensions.data()
    );

    auto [result, instance] = vk::createInstance(create_info);

    switch (result) {
    case vk::Result::eSuccess:
        instance_ = instance;
        break;
    default:
        spdlog::error(
            "Unable to create a Vulkan Instance - Error: {}", 
            vk::to_string(result)
        );
        break;
    }
}

Engine::~Engine() {
    instance_.destroy();
    glfwTerminate();
}

void Engine::run() const noexcept {
    while(!glfwWindowShouldClose(window_.handle)) {
        glfwPollEvents();
    }
} 

} // namespace nkgt