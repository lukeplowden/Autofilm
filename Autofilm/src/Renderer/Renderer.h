#pragma once
#include "Core/Core.h"
#include "Renderer/RenderAPI.h"

namespace Autofilm
{
    class AUTOFILM_API Renderer
    {
    public:
        static void init(RenderAPIType renderer);
        static void shutdown();
    private:
        static std::unique_ptr<RenderAPI> s_api;
    };
}