#include "autofilmpch.h"
#include "App.h"
#include "Log.h"
#include <glad/glad.h>
#include "Video/VideoPlayer.h"
#include <GLFW/glfw3.h>
#include "OpenGL/OpenGLWindow.h"

namespace Autofilm
{
    App::App(const ProjectSettings& settings)
    {
        _settings = settings;
    }

    App::~App()
    {
        
    }

    void App::Run()
    {
        hello_gst();
        while (m_running)
        {
            for (auto& window : _windows)
            {
                window->onUpdate();
            }
        }
    }
}  