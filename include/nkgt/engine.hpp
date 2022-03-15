#pragma once

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#endif
#include "vulkan/vulkan.hpp"

#include <string>

struct GLFWwindow;

namespace nkgt {

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
};

} // namespace nkgt