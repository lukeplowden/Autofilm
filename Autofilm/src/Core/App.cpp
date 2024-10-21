#include "autofilmpch.h"
#include "App.h"
#include "Log.h"
#include "Video/VideoPlayer.h"

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
        // hello_gst();
        while (m_running)
        {
            for (auto& window : WindowManager::getWindows())
            {
                Renderer::drawFrame();
                window->onUpdate();
            }
        }
    }
}  