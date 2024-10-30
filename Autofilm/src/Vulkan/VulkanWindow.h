#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>

#include "Core/Core.h"
#include "Core/Window.h"
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

            EventCallbackFn eventCallback;
        };

    public:
        WindowData getData() { return _data; }
        unsigned int getID() { return _data.ID; }
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

        GLFWwindow* _window;
    private: 
        virtual void init(const WindowProperties& props, unsigned int ID);
        virtual void shutdown();
        WindowData _data;
        friend class VulkanAPI;
        friend class VulkanWindowManager;
    };
}