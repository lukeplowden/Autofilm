#pragma once
#include "Core/Core.h"
#include <glm/glm.hpp>

namespace Autofilm
{
    enum class AUTOFILM_API RenderAPIType
    {
        Vulkan,
        None
        // that's all for now
    };

    class AUTOFILM_API RenderAPI
    {
    public:
        virtual void init() = 0;
        virtual void shutdown() = 0;
        virtual void drawFrame() = 0;
        virtual void clearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
    };
}