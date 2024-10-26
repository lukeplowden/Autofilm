#include "autofilmpch.h"
#include "Vulkan/VulkanWindow.h"
#include "Core/Log.h"
#include "Events/AllEvents.h"

namespace Autofilm
{
    static bool s_GLFWInitialised = false;

    VulkanWindow::VulkanWindow(const WindowProperties& props, unsigned int ID)
    {
        init(props, ID);
    }

    void VulkanWindow::init(const WindowProperties& props, unsigned int ID)
    {
        _data.title = props.title;
        _data.width = props.width;
        _data.height = props.height;
        _data.ID = ID;

        AF_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        _window = glfwCreateWindow(_data.width, _data.height, _data.title.c_str(), nullptr, nullptr);
        AF_CORE_ASSERT(_window, "Failed to create a GLFW window.");
        glfwSetWindowUserPointer(_window, &_data);

        glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
        {
            WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event(data.ID);
            data.eventCallback(event);
        });

        glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height)
        {
            WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;
            WindowResizeEvent event(width, height, data.ID);
            data.eventCallback(event);
        });

        glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action)
            {
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, true, data.ID);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, false, data.ID);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key, data.ID);
                    data.eventCallback(event);
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action)
            {
                case GLFW_PRESS:
                {
                MouseButtonPressedEvent event(button, data.ID);
                data.eventCallback(event);
                break;
                }
                case GLFW_RELEASE:
                {
                MouseButtonReleasedEvent event(button, data.ID);
                data.eventCallback(event);
                break;
                }
            }
        });

        glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xpos, double ypos)
        {
            WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event(xpos, ypos, data.ID);
            data.eventCallback(event);
        });
    }

    VulkanWindow::~VulkanWindow()
    {
        shutdown();
    }

    void VulkanWindow::shutdown()
    {
        // vkDestroySurfaceKHR()
        // for (auto imageView : _data.swapChainImageViews) {
            // vkDestroyImageView(device, imageView, nullptr);
        // }
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
