#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.hpp"

struct GLFWwindow;

namespace nkgt {

struct SwapChainDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> present_modes;

    SwapChainDetails(const vk::PhysicalDevice& physical_device,
                     const vk::SurfaceKHR& surface);

    [[nodiscard]] vk::PresentModeKHR best_present_mode() const noexcept;
    [[nodiscard]] vk::SurfaceFormatKHR best_format() const noexcept;
    [[nodiscard]] vk::Extent2D extent(GLFWwindow* window) const noexcept;
    [[nodiscard]] std::pair<uint32_t, uint32_t> image_count() const noexcept;
};

} // namespace nkgt