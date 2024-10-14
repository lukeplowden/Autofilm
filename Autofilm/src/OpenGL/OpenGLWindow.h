#pragma once

#include "Core/Core.h"
#include "Core/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

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
        void setFullscreen(bool fullscreen) override;
        bool isFullscreen() const override;
        void setVSync(bool enabled) override;
        bool isVSync() const override;
        GLFWwindow* getWindow() const;

    private:
        virtual void init(const WindowProperties& props);
        virtual void shutdown();

        GLFWwindow* m_window {nullptr};
        GLFWmonitor* m_monitor {nullptr};

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