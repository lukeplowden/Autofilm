#pragma once
#include "Renderer/RenderAPI.h"

namespace Autofilm
{
    class AUTOFILM_API VulkanAPI : public RenderAPI
    {
    public:
        void init() override;
        void shutdown() override;
        void clearColor(const glm::vec4& color) override;
		void clear() override;
    };
}