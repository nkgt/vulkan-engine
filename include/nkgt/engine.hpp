#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.hpp"

#include <optional>
#include <string>

struct GLFWwindow;

namespace nkgt {

struct QueueFamilies {
    std::optional<uint32_t> graphic_family;

    [[nodiscard]] bool is_complete() const noexcept;
};

class Engine {
public:
    Engine(std::string window_name, int width, int height);
    ~Engine();

    void run() const noexcept;

private:
    struct WindowInfo {
        std::string name;
        int width;
        int height;
        GLFWwindow* handle;
    };

    WindowInfo window_;
    
    vk::Instance instance_;
#ifndef NDEBUG
    vk::DebugUtilsMessengerEXT debug_messenger_;
#endif

    vk::PhysicalDevice physical_device_;
    QueueFamilies queue_families_;
};

} // namespace nkgt