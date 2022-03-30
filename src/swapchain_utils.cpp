#include "nkgt/swapchain_utils.hpp"

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace nkgt {

SwapChainDetails::SwapChainDetails(const vk::PhysicalDevice& physical_device,
                                   const vk::SurfaceKHR& surface) {
    auto [capabilities_result, temp_capabilities] =
        physical_device.getSurfaceCapabilitiesKHR(surface);

    if (capabilities_result == vk::Result::eSuccess) {
        capabilities = temp_capabilities;
    } else {
        spdlog::error(
            "Unable to retrieve surface capabilities - Error {}",
            vk::to_string(capabilities_result)
        );
    }

    auto [formats_result, temp_formats] =
        physical_device.getSurfaceFormatsKHR(surface);

    if (formats_result == vk::Result::eSuccess) {
        formats = temp_formats;
    } else {
        spdlog::error(
            "Unable to retrieve surface formats - Error {}",
            vk::to_string(formats_result)
        );
    }

    auto [modes_results, temp_modes] =
        physical_device.getSurfacePresentModesKHR(surface);

    if (modes_results == vk::Result::eSuccess) {
        present_modes = temp_modes;
    } else {
        spdlog::error(
            "Unable to retrieve surface presentations modes - Error {}",
            vk::to_string(modes_results)
        );
    }
}

[[nodiscard]] vk::PresentModeKHR
SwapChainDetails::best_present_mode() const noexcept {
    for (const auto& mode : present_modes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            return mode;
        }
    }

    spdlog::warn(
        "Unable to find {} present mode. Falling back to {}",
        vk::to_string(vk::PresentModeKHR::eMailbox),
        vk::to_string(vk::PresentModeKHR::eFifo)
    );

    return vk::PresentModeKHR::eFifo;
}

[[nodiscard]] vk::SurfaceFormatKHR
SwapChainDetails::best_format() const noexcept {
    for (const auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    // The Vulkan specification ensures that there must always be atleast one
    // supported format. See:
    // https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#_surface_format_support
    spdlog::warn(
        "Unable to find sRGB surface format and color space."
        "Falling back to {} format and {} color space",
        vk::to_string(formats[0].format),
        vk::to_string(formats[0].colorSpace)
    );

    return formats[0];
}

[[nodiscard]] vk::Extent2D
SwapChainDetails::extent(GLFWwindow* window) const noexcept {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        vk::Extent2D extent = {
            std::clamp(
                static_cast<uint32_t>(width),
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width
            ),
            std::clamp(
                static_cast<uint32_t>(height),
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height
            )
        };

        return extent;
    }
}

[[nodiscard]] std::pair<uint32_t, uint32_t>
SwapChainDetails::image_count() const noexcept {
    return { capabilities.minImageCount, capabilities.maxImageCount };
}

} // namespace nkgt