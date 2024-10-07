#include "OpenGLWindow.h"

namespace Autofilm
{
    OpenGLWindow::OpenGLWindow()
    {
    }
    
    OpenGLWindow::~OpenGLWindow()
    {

    }

    void OpenGLWindow::init(int width, int height)
    {

    }
    
    void OpenGLWindow::mainLoop()
    {

    }

    bool OpenGLWindow::isFullscreen()
    {
        return glfwGetWindowMonitor(_wnd) != nullptr;
    }

    void OpenGLWindow::setFullscreen(bool fullscreen)
    {

    }
}
