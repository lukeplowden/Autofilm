#pragma once

#include "Core/Core.h"
#include <variant>

namespace Autofilm
{
    class Event
    {
    public:
        enum struct Type 
        { 
            Keyboard, Mouse, App 
        };

        Type getType() const { return type; }

    private:
        Type type;
    };
}