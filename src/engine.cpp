#include "nkgt/engine.hpp"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

namespace nkgt {

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

#ifndef NDEBUG
    debug_messenger_ = create_dbg_messenger(instance_);
#endif

    physical_device_ = create_physical_device(instance_);
}

Engine::~Engine() {
#ifndef NDEBUG
    instance_.destroyDebugUtilsMessengerEXT(debug_messenger_, nullptr);
#endif
    instance_.destroy();

    glfwTerminate();
}

void Engine::run() const noexcept {
    while(!glfwWindowShouldClose(window_.handle)) {
        glfwPollEvents();
    }
} 

} // namespace nkgt