#include "autofilmpch.h"
#include "OpenGL/OpenGLWindow.h"
#include "Core/Log.h"

namespace Autofilm
{
    static bool s_GLFWInitialised = false;

    OpenGLWindow::OpenGLWindow(const WindowProperties& props)
    {
        init(props);
    }

    OpenGLWindow::~OpenGLWindow()
    {
        shutdown();
    }

    void OpenGLWindow::init(const WindowProperties& props)
    {
        _data.title = props.title;
        _data.width = props.width;
        _data.height = props.height;

        AF_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

        if (!s_GLFWInitialised)
        {
            int success = glfwInit();
            AF_CORE_ASSERT(success, "Could not initialise GLFW.");

            s_GLFWInitialised = true;
        }
        _window = glfwCreateWindow((int)props.width, (int)props.height, props.title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(_window);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        AF_CORE_ASSERT(status, "Failed to initalise Glad!");
        glfwSetWindowUserPointer(_window, &_data);
        setVSync(true);
    }

    void OpenGLWindow::shutdown()
    {
        glfwDestroyWindow(_window);
    }

    void OpenGLWindow::onUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(_window);
    }

    void OpenGLWindow::setVSync(bool enabled)
    {
        if (enabled){
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        _data.VSync = enabled;
    }

    bool OpenGLWindow::isVSync() const
    {
        return _data.VSync;
    }

    void OpenGLWindow::setFullscreen(bool fullscreen)
    {
        return;
    }

    bool OpenGLWindow::isFullscreen() const
    {
        return _data.fullscreen;
    }

    GLFWwindow* OpenGLWindow::getWindow() const
    {
        return _window;
    }
}
