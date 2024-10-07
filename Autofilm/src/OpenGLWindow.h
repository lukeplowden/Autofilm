#pragma once

#include "afpch.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

namespace Autofilm
{

    class OpenGLWindow
    {
    public:
        OpenGLWindow();
        ~OpenGLWindow();

        void init(int width, int height);
        static void callbackResize(GLFWwindow* window, int cx, int cy);
        void mainLoop();
        bool isFullscreen();
        void setFullscreen(bool fullscreen);

    private:
        std::array<int, 2> _wndPos {0, 0};
        std::array<int, 2> _wndSize {0, 0};
        std::array<int,  2> _vpSize {0, 0};
        bool _updateViewport {true};
        GLFWwindow* _wnd {nullptr};
        GLFWmonitor* _monitor {nullptr};

        void resize(int cx, int cy);
    };
    
}