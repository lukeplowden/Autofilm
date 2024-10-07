#pragma once
#include "autofilmpch.h"

#include "Core/Core.h"
#include "Core/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Autofilm
{

    class OpenGLWindow : public Window
    {
    public:
        OpenGLWindow(const WindowProperties& props);
        virtual ~OpenGLWindow();


        void onUpdate() override;

        inline unsigned int getWidth() const override { return m_data.width; };
        inline unsigned int getHeight() const override { return m_data.height; };

        // Window Attributes
        // inline void setEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; };
        void setFullscreen(bool fullscreen) override;
        bool isFullscreen() const override;
        void setVSync(bool enabled) override;
        bool isVSync() const override;

    private:
        virtual void init(const WindowProperties& props);
        virtual void shutdown();


    private:
        GLFWwindow* m_window {nullptr};
        GLFWmonitor* m_monitor {nullptr};

        struct WindowData
        {
            std::string title;
            unsigned int width, height;
            bool VSync;
            bool fullscreen;
            // EventCallbackFn eventCallback;
        };

        WindowData m_data;
    };
    
}