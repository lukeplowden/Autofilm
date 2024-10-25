#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Core/Core.h"
#include "Core/Window.h"
#include <string>
#include "Events/Event.h"

namespace Autofilm
{
    class VulkanAPI;

    class AUTOFILM_API VulkanWindow : public Window
    {
    private: 
        struct WindowData
        {
            std::string title;
            unsigned int width, height;
            bool VSync;
            bool fullscreen;
            unsigned int ID;

            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            std::vector<VkImage> swapchainImages;
            VkFormat swapchainImageFormat;
            VkExtent2D swapchainExtent;
            std::vector<VkImageView> swapchainImageViews;
            std::vector<VkFramebuffer> swapchainFramebuffers;

            EventCallbackFn eventCallback;
        };

    public:
        WindowData getData() { return _data; }

        VulkanWindow(const WindowProperties& props, unsigned int ID);
        virtual ~VulkanWindow();

        void onUpdate() override;

        inline unsigned int getWidth() const override { return _data.width; };
        inline unsigned int getHeight() const override { return _data.height; };

        void setEventCallback(const EventCallbackFn& callback) override { _data.eventCallback = callback; }

        // Window Attributes
        void setFullscreen(bool fullscreen) override;
        bool isFullscreen() const override;
        void setVSync(bool enabled) override;
        bool isVSync() const override;
        // Vulkan specifics
        // Surface
        void createSurface(VkInstance& instance);
        void destroySurface(VkInstance& instance);
        VkSurfaceKHR getSurface() { return _data.surface; }
        // Swapchain
        void createSwapchain(VkDevice& device, VkSwapchainCreateInfoKHR& createInfo, uint32_t imageCount, VkFormat imageFormat, VkExtent2D extent);
        // Image views
        void createImageViews(VkDevice& device);
        // Framebuffers
        void createFramebuffers(VkDevice& device, VkRenderPass& renderPass);

    private: 
        virtual void init(const WindowProperties& props, unsigned int ID);
        virtual void shutdown();
        GLFWwindow* _window;
        WindowData _data;
        friend class VulkanAPI;
    };
}