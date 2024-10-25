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

    void VulkanWindow::createSurface(VkInstance& instance)
    {
        VkResult result = glfwCreateWindowSurface(instance, _window, nullptr, &_data.surface);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a VkSurface.");
    }

    void VulkanWindow::destroySurface(VkInstance& instance)
    {
        vkDestroySurfaceKHR(instance, _data.surface, nullptr);
    }

    void VulkanWindow::createSwapchain(VkDevice& device, VkSwapchainCreateInfoKHR& createInfo, uint32_t imageCount, VkFormat imageFormat, VkExtent2D extent)
    {
        VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &_data.swapchain);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan Swapchain.");
        vkGetSwapchainImagesKHR(device, _data.swapchain, &imageCount, nullptr);
        _data.swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, _data.swapchain, &imageCount, _data.swapchainImages.data());
        _data.swapchainImageFormat = imageFormat;
        _data.swapchainExtent = extent;
    }

    void VulkanWindow::createImageViews(VkDevice& device)
    {
        _data.swapchainImageViews.resize(
            _data.swapchainImages.size()
        );
        for (size_t i = 0; i < _data.swapchainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _data.swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _data.swapchainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            VkResult result = vkCreateImageView(device, &createInfo, nullptr, &_data.swapchainImageViews[i]);
            AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create an image view");
        }
    }

    void VulkanWindow::createFramebuffers(VkDevice& device, VkRenderPass& renderPass)
    {
        const auto imageViews = _data.swapchainImageViews;
        _data.swapchainFramebuffers.resize(imageViews.size());
        for (size_t i = 0; i < imageViews.size(); i++) {
            VkImageView attachments[] = {
                imageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = _data.swapchainExtent.width;
            framebufferInfo.height = _data.swapchainExtent.height;
            framebufferInfo.layers = 1;
            
            VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &_data.swapchainFramebuffers[i]);
            AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a framebuffer.");       
        }
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
