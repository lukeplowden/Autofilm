#include "autofilmpch.h"
#include "OpenGL/OpenGLWindow.h"
#include "Core/Log.h"

namespace Autofilm
{
    static bool s_GLFWInitialised = false;

    Window* Window::Create(const WindowProperties& props)
    {
        return new OpenGLWindow(props);
    }
 
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
        m_data.title = props.title;
        m_data.width = props.width;
        m_data.height = props.height;

        AF_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

        if (!s_GLFWInitialised)
        {
            int success = glfwInit();
            AF_CORE_ASSERT(success, "Could not initialise GLFW.");

            s_GLFWInitialised = true;
        }
        m_window = glfwCreateWindow((int)props.width, (int)props.height, props.title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_window);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        AF_CORE_ASSERT(status, "Failed to initalise Glad!");
        glfwSetWindowUserPointer(m_window, &m_data);
        setVSync(true);
    }

    void OpenGLWindow::shutdown()
    {
        glfwDestroyWindow(m_window);
    }

    void OpenGLWindow::onUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void OpenGLWindow::setVSync(bool enabled)
    {
        if (enabled){
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        m_data.VSync = enabled;
    }

    bool OpenGLWindow::isVSync() const
    {
        return m_data.VSync;
    }

    void OpenGLWindow::setFullscreen(bool fullscreen)
    {
        return;
    }

    bool OpenGLWindow::isFullscreen() const
    {
        return m_data.fullscreen;
    }

    GLFWwindow* OpenGLWindow::getWindow() const
    {
        return m_window;
    }
}
