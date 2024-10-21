#pragma once

#include "Core/Core.h"

namespace Autofilm
{
    class AUTOFILM_API IRenderable
    {
    public:
        virtual void draw() = 0;
    };
}