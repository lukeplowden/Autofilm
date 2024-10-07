#pragma once
#include "Core.h"
#include "Window.h"

namespace Autofilm
{
    class AUTOFILM_API App
    {
    public:
        App();
        virtual ~App();

        void Run();
    private:
        std::unique_ptr<Window> m_window;
        bool m_running = true;
    };

    App* CreateApp();
}