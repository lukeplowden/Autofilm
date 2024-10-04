#pragma once
#include "Core.h"
#include "Window.h"

namespace Autofilm
{
    class AF_API App
    {
    public:
        App();
        virtual ~App();

        void Run();
        auto NewFilm() {}
    private:
        bool running = true;
        float lastFrameTime = 0.0f;
    };

    App* CreateApp();
}