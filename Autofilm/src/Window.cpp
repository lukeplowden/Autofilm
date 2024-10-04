#include "Window.h"

namespace Autofilm
{
    Window::Window(/* args */)
    {
    }
    
    Window::~Window()
    {

    }

    void Window::init(int width, int height)
    {

    }
    
    void Window::mainLoop()
    {

    }

    bool Window::isFullscreen()
    {
        return glfwGetWindowMonitor(_wnd) != nullptr;
    }

    void Window::setFullscreen(bool fullscreen)
    {

    }
}
