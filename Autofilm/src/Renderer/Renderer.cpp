#include "autofilmpch.h"
#include "Renderer/Renderer.h"
#include "Core/Log.h"
#include "OpenGL/OpenGLAPI.h"

namespace Autofilm 
{
    std::unique_ptr<RenderAPI> Renderer::s_api = nullptr;

    void Renderer::init(RenderAPIType renderer)
    {
        switch (renderer)
        {
            case RenderAPIType::OpenGL:
                s_api = std::make_unique<OpenGLAPI>();
                break;
        }
        AF_CORE_ASSERT(s_api, "Invalid renderer choice.");
        s_api->init();
        s_api->clearColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
    }

    void Renderer::shutdown()
    {
        if (s_api)
        {
            s_api->shutdown();
            s_api.reset();
        }
    }
}