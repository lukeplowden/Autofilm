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

        inline unsigned int getWidth() const override { return _data.width; };
        inline unsigned int getHeight() const override { return _data.height; };

        // Window Attributes
        void setFullscreen(bool fullscreen) override;
        bool isFullscreen() const override;
        void setVSync(bool enabled) override;
        bool isVSync() const override;
        GLFWwindow* getWindow() const;

    private:
        virtual void init(const WindowProperties& props);
        virtual void shutdown();

        GLFWwindow* _window {nullptr};
        GLFWmonitor* _monitor {nullptr};

        struct WindowData
        {
            std::string title;
            unsigned int width, height;
            bool VSync;
            bool fullscreen;
        };

        WindowData _data;
    };
    
}