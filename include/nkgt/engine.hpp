#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.hpp"

#include <optional>
#include <string>
#include <vector>

struct GLFWwindow;

namespace nkgt {

struct QueueFamilies {
    std::optional<uint32_t> graphic_family;
    std::optional<uint32_t> present_family;

    [[nodiscard]] bool is_complete() const noexcept;
    [[nodiscard]] std::vector<uint32_t> unique_indices() const noexcept;
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

    vk::Device device_;
    vk::Queue graphic_queue_;
    vk::Queue present_queue_;

    vk::SurfaceKHR surface_;
    vk::SwapchainKHR swapchain_;
    std::vector<vk::Image> swapchain_images_;
};

} // namespace nkgt