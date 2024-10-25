#include "autofilmpch.h"
#include "App.h"
#include "Log.h"
#include "Video/VideoPlayer.h"
#include <thread>

namespace Autofilm
{
    App::App(const ProjectSettings& settings)
    {
        _settings = settings;

        Log::Init();
	    AF_CORE_INFO("Initialised Log!");
	    AF_INFO("Initialised Log!");

        WindowManager::Init(_settings.renderer);
        for (int i = 0; i < _settings.numWindows; i++) {
	        WindowManager::createWindow();
        }
        Renderer::init(_settings.renderer);
    }

    App::~App()
    {
        
    }

    void App::Run()
    {
        // std::thread t { hello_gst };
        // std::thread tt { hello_gst };
        // std::thread ttt { hello_gst };
        // std::thread tttt { hello_gst };
        // hello_gst();
        while (m_running)
        {
            Renderer::drawFrame();
            for (auto& window : WindowManager::getWindows())
            {
                window->onUpdate();
            }
        }
    }
}  