#pragma once

#include <GLFW/glfw3.h>
#include "Core/Core.h"
#include "Core/Window.h"
#include <string>

namespace Autofilm
{
    class AUTOFILM_API VulkanWindow : public Window
    {
    public:
        VulkanWindow(const WindowProperties& props);
        virtual ~VulkanWindow();

        void onUpdate() override;

        inline unsigned int getWidth() const override { return m_data.width; };
        inline unsigned int getHeight() const override { return m_data.height; };

        // Window Attributes
        void setFullscreen(bool fullscreen) override;
        bool isFullscreen() const override;
        void setVSync(bool enabled) override;
        bool isVSync() const override;
    
    private: 
        virtual void init(const WindowProperties& props);
        virtual void shutdown();
    
        struct WindowData
        {
            std::string title;
            unsigned int width, height;
            bool VSync;
            bool fullscreen;
        };

        WindowData m_data;
    };
}