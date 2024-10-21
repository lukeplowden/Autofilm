#pragma once
#include "Core/Core.h"
#include "Core/Window.h"
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
        bool multiChannel;

        ProjectSettings(
                    const std::string& title = "Autofilm", 
                    bool multiChannel = true,
                    RenderAPIType renderer = RenderAPIType::Vulkan
                )
            : title(title), multiChannel(multiChannel), renderer(renderer)
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