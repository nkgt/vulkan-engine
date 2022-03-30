#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.hpp"

struct GLFWwindow;

namespace nkgt {

struct SwapChainDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    vk::SurfaceFormatKHR format;
    vk::PresentModeKHR present_mode;
    vk::Extent2D extent;

    SwapChainDetails() = default;
    SwapChainDetails(const vk::PhysicalDevice& physical_device,
                     const vk::SurfaceKHR& surface,
                     GLFWwindow* window);
};

} // namespace nkgt