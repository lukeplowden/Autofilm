#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Core/Core.h"
#include "Core/Window.h"
#include <string>

namespace Autofilm
{
    class AUTOFILM_API VulkanWindow : public Window
    {
    private: 
        struct WindowData
        {
            std::string title;
            unsigned int width, height;
            bool VSync;
            bool fullscreen;
            VkSurfaceKHR surface;
        };

    public:
        WindowData getData() { return _data;};

        VulkanWindow(const WindowProperties& props);
        virtual ~VulkanWindow();

        void onUpdate() override;

        inline unsigned int getWidth() const override { return _data.width; };
        inline unsigned int getHeight() const override { return _data.height; };

        // Window Attributes
        void setFullscreen(bool fullscreen) override;
        bool isFullscreen() const override;
        void setVSync(bool enabled) override;
        bool isVSync() const override;
        // Vulkan specifics
        void createSurface(VkInstance& instance);
        void destroySurface(VkInstance& instance);
        VkSurfaceKHR getSurface() { return _data.surface; }

    private: 
        virtual void init(const WindowProperties& props);
        virtual void shutdown();
        GLFWwindow* _window;
        WindowData _data;
    };
}