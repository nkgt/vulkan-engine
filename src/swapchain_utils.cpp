#include "nkgt/swapchain_utils.hpp"

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <limits>

namespace nkgt {

SwapChainDetails::SwapChainDetails(const vk::PhysicalDevice& physical_device,
                                   const vk::SurfaceKHR& surface,
                                   GLFWwindow* window) {
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

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        extent = capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        extent = vk::Extent2D {
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
    }

    auto [formats_result, formats] =
        physical_device.getSurfaceFormatsKHR(surface);

    if (formats_result == vk::Result::eSuccess) {
        bool found_srgb = false;
        for (const auto& f : formats) {
            if (f.format == vk::Format::eB8G8R8A8Srgb &&
                f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                found_srgb = true;
                format = f;
            }
        }

        if (!found_srgb) {
            // The Vulkan specification ensures that there must always be atleast one
            // supported format. See:
            // https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#_surface_format_support
            spdlog::warn(
                "Unable to find sRGB surface format and color space."
                "Falling back to {} format and {} color space",
                vk::to_string(formats[0].format),
                vk::to_string(formats[0].colorSpace)
            );

            format = formats[0];
        }
    } else {
        spdlog::error(
            "Unable to retrieve surface formats - Error {}",
            vk::to_string(formats_result)
        );
    }

    auto [modes_results, present_modes] =
        physical_device.getSurfacePresentModesKHR(surface);

    if (modes_results == vk::Result::eSuccess) {
        bool found_mailbox = false;
        for (const auto& mode : present_modes) {
            if (mode == vk::PresentModeKHR::eMailbox) {
                found_mailbox = true;
                present_mode = mode;
            }
        }

        if (!found_mailbox) {
            spdlog::warn(
                "Unable to find {} present mode. Falling back to {}",
                vk::to_string(vk::PresentModeKHR::eMailbox),
                vk::to_string(vk::PresentModeKHR::eFifo)
            );

            present_mode = vk::PresentModeKHR::eFifo;
        }
    } else {
        spdlog::error(
            "Unable to retrieve surface presentations modes - Error {}",
            vk::to_string(modes_results)
        );
    }
}

} // namespace nkgt