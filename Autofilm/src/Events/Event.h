#pragma once

#include "Core/Core.h"
#include <string>
#include <functional>

namespace Autofilm
{
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory
    {
      None = 0,
      EventCategoryApplication  = BIT(0),  
      EventCategoryInput        = BIT(1),  
      EventCategoryKeyboard     = BIT(2),  
      EventCategoryMouse        = BIT(3),  
      EventCategoryMouseButton  = BIT(4),  
    };

    class Event
    {
        friend class EventDispatcher;
    public:
        enum struct Type 
        { 
            Keyboard, Mouse, App 
        };

    protected:
        bool _handled;
    };

    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
    public:
        EventDispatcher(Event& event)
            : _event(event)
        {
        }

        template <typename T>
        bool dispatch(EventFn<T> func)
        {
            if(_event.getEventType() == T::getStaticType()){
                _event.m_Handled = func(*(T*)&m_event);
                return true;
            }
            return false;
        }
    private:
        Event& _event;
    };
}