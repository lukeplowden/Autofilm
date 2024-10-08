#pragma once
#include "Core.h"
#include "Window.h"
#include "Renderer/Renderer.h"
#include <vector>
#include <memory>
#include <string>

namespace Autofilm
{
    struct AUTOFILM_API AppProperties
    {
        std::string title;
        RenderAPIType renderer;
        bool multiWindow;

        AppProperties(const std::string& title = "Autofilm", 
                    bool multiWindow = true,
                    RenderAPIType renderer = RenderAPIType::OpenGL)
            : title(title), multiWindow(multiWindow), renderer(renderer)
        {}
    };

    class AUTOFILM_API App
    {
    public:
        void Run();
        virtual ~App();
    
    protected:
        App(const AppProperties& props = AppProperties());

    private:
        App(const App&) = delete;
        App& operator=(const App&) = delete;

        std::vector<std::unique_ptr<Window>> m_windows;
        bool m_running = true;
        AppProperties m_props;
    };

    App* CreateApp();
}