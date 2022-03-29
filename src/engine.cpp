#include "nkgt/engine.hpp"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

namespace nkgt {

[[nodiscard]] bool QueueFamilies::is_complete() const noexcept {
    return graphic_family.has_value() &&
           present_family.has_value();
}

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData) {
    switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        spdlog::warn(pCallbackData->pMessage );
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        spdlog::error(pCallbackData->pMessage );
        break;
    default:
        spdlog::info(pCallbackData->pMessage );
        break;
    }

    return VK_FALSE;
}

[[nodiscard]] static vk::DebugUtilsMessengerEXT
create_dbg_messenger(const vk::Instance& instance) noexcept {
    auto type_flags = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral    |
                      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    auto severity_flags = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    vk::DebugUtilsMessengerCreateInfoEXT debug_info(
        {},
        severity_flags,
        type_flags,
        debug_callback
    );

    auto [result, messenger] = instance.createDebugUtilsMessengerEXT(
        debug_info,
        nullptr
    );

    if (result == vk::Result::eSuccess) {
        return messenger;
    } else {
        spdlog::error(
            "Unable to create debug messenger - Error {}",
            vk::to_string(result)
        );
        return {};
    }
}
#endif

void glfw_error_callback(int error_code, const char* error_description) {
    spdlog::error("GLFW - {} (Code {})", error_description, error_code);
}

[[nodiscard]] static vk::Instance
create_instance(std::string_view name) noexcept {
    vk::ApplicationInfo app_info(
        name.data(), 
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

    if (result == vk::Result::eSuccess) {
        return instance;
    } else {
        spdlog::error(
            "Unable to create a Vulkan Instance - Error: {}", 
            vk::to_string(result)
        );

        return {};
    }
}

[[nodiscard]] static vk::PhysicalDevice
create_physical_device(const vk::Instance& instance) noexcept {
    auto [result, available_devices] = instance.enumeratePhysicalDevices();

    if (result == vk::Result::eSuccess) {
        for (const auto& device : available_devices) {
            if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
                return device;
            }
        }

        spdlog::error("Unable to find a discrete GPU in the system.");

        return {};
    } else {
        spdlog::error(
            "Unable to enumerate physical devices - Error: {}",
            vk::to_string(result)
        );

        return {};
    }
}

[[nodiscard]] static QueueFamilies
get_queue_families(const vk::PhysicalDevice& physical_device,
                   const vk::SurfaceKHR& surface) noexcept {
    auto available_families = physical_device.getQueueFamilyProperties();
    QueueFamilies families;

    for (uint32_t i = 0; i < available_families.size(); ++i) {
        if (available_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            families.graphic_family = i;
        }

        if (physical_device.getSurfaceSupportKHR(i, surface).value) {
            families.present_family = i;
        }

        if (families.is_complete()) {
            break;
        }
    }

    return families;
}

[[nodiscard]] static vk::Device
create_device(const vk::PhysicalDevice& physical_device, 
              const QueueFamilies& queue_families) noexcept {
    float priority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    queue_infos.reserve(2);
    
    vk::DeviceQueueCreateInfo graphic_queue_info(
        {},
        *queue_families.graphic_family,
        1,
        &priority
    );
    queue_infos.push_back(graphic_queue_info);

    // The queue family that has been selected for presentation could be the
    // same as the graphic one and we should not specify the same family twice
    // during device creation. See:
    // https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#VUID-VkDeviceCreateInfo-queueFamilyIndex-02802
    if(*queue_families.graphic_family != *queue_families.present_family) {
        vk::DeviceQueueCreateInfo present_queue_info(
            {}, 
            *queue_families.present_family, 
            1, 
            &priority
        );

        queue_infos.push_back(present_queue_info);
    }

    auto [result, device] = physical_device.createDevice({ {}, queue_infos });

    if (result == vk::Result::eSuccess) {
        return device;
    } else {
        spdlog::error(
            "Unable to create a logical device - Error: {}",
            vk::to_string(result)
        );

        return {};
    }
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

    vk::DynamicLoader dynamic_loader;
    auto get_proc_addr_fn = dynamic_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(get_proc_addr_fn);

    instance_ = create_instance(window_.name);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance_);

    glfwCreateWindowSurface(
        VkInstance(instance_), 
        window_.handle, 
        nullptr, 
        reinterpret_cast<VkSurfaceKHR*>(&surface_)
    );

#ifndef NDEBUG
    debug_messenger_ = create_dbg_messenger(instance_);
#endif

    physical_device_ = create_physical_device(instance_);
    queue_families_ = get_queue_families(physical_device_, surface_);

    if (!queue_families_.is_complete()) {
        spdlog::error("Unable to find all needed queue families.");
    }

    device_ = create_device(physical_device_, queue_families_);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device_);
    graphic_queue_ = device_.getQueue(*queue_families_.graphic_family, 0);
    present_queue_ = device_.getQueue(*queue_families_.present_family, 0);
}

Engine::~Engine() {
    device_.destroy();

#ifndef NDEBUG
    instance_.destroyDebugUtilsMessengerEXT(debug_messenger_, nullptr);
#endif
    instance_.destroySurfaceKHR(surface_);
    instance_.destroy();

    glfwTerminate();
}

void Engine::run() const noexcept {
    while(!glfwWindowShouldClose(window_.handle)) {
        glfwPollEvents();
    }
} 

} // namespace nkgt