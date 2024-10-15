#pragma once
#include "Core.h"
#include "Window.h"
#include "Renderer/Renderer.h"
#include <vector>
#include <memory>
#include <string>

namespace Autofilm
{
    struct AUTOFILM_API ProjectSettings
    {
        std::string title;
        RenderAPIType renderer;
        bool multiWindow;

        ProjectSettings(const std::string& title = "Autofilm", 
                    bool multiWindow = true,
                    RenderAPIType renderer = RenderAPIType::OpenGL)
            : title(title), multiWindow(multiWindow), renderer(renderer)
        {}
    };

    class AUTOFILM_API App
    {
    public:
        ProjectSettings _settings;
        void Run();
        virtual ~App();
        virtual void setup() = 0;
    
    protected:
        App(const ProjectSettings& props = ProjectSettings());

    private:
        App(const App&) = delete;
        App& operator=(const App&) = delete;
        bool m_running = true;
        std::vector<std::unique_ptr<Window>> _windows;

    };

    App* CreateApp();
}