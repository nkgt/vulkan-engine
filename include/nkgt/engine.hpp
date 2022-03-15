#pragma once

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
};

} // namespace nkgt