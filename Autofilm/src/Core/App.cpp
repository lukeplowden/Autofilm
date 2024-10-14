#include "autofilmpch.h"
#include "App.h"
#include "Log.h"
#include <glad/glad.h>
#include "Video/VideoPlayer.h"
#include <GLFW/glfw3.h>
#include "OpenGL/OpenGLWindow.h"

namespace Autofilm
{
    App::App(const AppProperties& props)
    {
        m_props = props;
    }

    App::~App()
    {

    }

    void App::Run()
    {
        m_windows.push_back(std::unique_ptr<Window>(Window::Create()));
        // m_windows.push_back(std::unique_ptr<Window>(Window::Create()));
        // m_windows.push_back(std::unique_ptr<Window>(Window::Create()));
        hello_gst();
        while (m_running)
        {
            float i = 0.0f;
            for (auto& window : m_windows)
            {
                OpenGLWindow* openglWindow = dynamic_cast<OpenGLWindow*>(window.get()); 
                glfwMakeContextCurrent(openglWindow->getWindow());
                window->onUpdate();
                glClearColor(i, 0.0f, 0.5f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                i += 0.1f;
            }
        }
    }
}  