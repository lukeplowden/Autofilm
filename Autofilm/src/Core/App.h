#pragma once
#include "Core.h"
#include "OpenGLWindow.h"

namespace Autofilm
{
    class AUTOFILM_API App
    {
    public:
        App();
        virtual ~App();

        void Run();
    private:
        bool running = true;
        float lastFrameTime = 0.0f;
    };

    App* CreateApp();
}