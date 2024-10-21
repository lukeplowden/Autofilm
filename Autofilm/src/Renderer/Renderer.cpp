#include "autofilmpch.h"
#include "Renderer/Renderer.h"
#include "Core/Log.h"
#include "Vulkan/VulkanAPI.h"

namespace Autofilm 
{
    std::unique_ptr<RenderAPI> Renderer::s_api = nullptr;

    void Renderer::init(RenderAPIType renderer)
    {
        switch (renderer)
        {
            case RenderAPIType::Vulkan:
                s_api = std::make_unique<VulkanAPI>();
                break;
        }
        AF_CORE_ASSERT(s_api, "Invalid renderer choice.");
        s_api->init();
    }

    void Renderer::shutdown()
    {
        s_api->shutdown();
    }
    
    void Renderer::drawFrame()
    {
        s_api->drawFrame();
    } 
}