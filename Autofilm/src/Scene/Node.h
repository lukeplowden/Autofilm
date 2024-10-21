#pragma once

#include "Renderer/IRenderable.h"
#include <glm/glm.hpp>

namespace Autofilm
{
    class AUTOFILM_API Node : public IRenderable
    {
    public:
        void draw() override { for (auto& c : _children) { c->draw(); } }
    private: 
        std::weak_ptr<Node> _parent;
        std::vector<std::shared_ptr<Node>> _children;
        glm::mat4 localTransform;
        glm::mat4 worldTransform;
    };
}