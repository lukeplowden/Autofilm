#include "autofilmpch.h"
#include "Vulkan/VulkanWindow.h"
#include "Core/Log.h"

namespace Autofilm
{
    static bool s_GLFWInitialised = false;

    VulkanWindow::VulkanWindow(const WindowProperties& props)
    {
        init(props);
    }

    void VulkanWindow::init(const WindowProperties& props)
    {
        _data.title = props.title;
        _data.width = props.width;
        _data.height = props.height;

        AF_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        _window = glfwCreateWindow(_data.width, _data.height, _data.title.c_str(), nullptr, nullptr);
        AF_CORE_ASSERT(_window, "Failed to create a GLFW window.");
        glfwSetWindowUserPointer(_window, &_data);
    }

    void VulkanWindow::createSurface(VkInstance& instance)
    {
        VkResult result = glfwCreateWindowSurface(instance, _window, nullptr, &_data.surface);
        AF_VK_ASSERT_EQUAL(result, VK_SUCCESS, "Failed to create a VkSurface.");
    }

    void VulkanWindow::destroySurface(VkInstance& instance)
    {
        vkDestroySurfaceKHR(instance, _data.surface, nullptr);
    }

    VulkanWindow::~VulkanWindow()
    {
        shutdown();
    }

    void VulkanWindow::shutdown()
    {
        glfwDestroyWindow(_window);
    }

    void VulkanWindow::onUpdate()
    {
        glfwPollEvents();
    }

    void VulkanWindow::setVSync(bool enabled)
    {
        if (enabled){
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        _data.VSync = enabled;
    }

    bool VulkanWindow::isVSync() const
    {
        return _data.VSync;
    }

    void VulkanWindow::setFullscreen(bool fullscreen)
    {
        return;
    }

    bool VulkanWindow::isFullscreen() const
    {
        return _data.fullscreen;
    }
}
