#pragma once

#include "Core/Core.h"
#include <string>
#include <functional>

namespace Autofilm
{
    class BaseEvent
    {
    public:
        virtual ~BaseEvent() {};
    protected:
        static size_t getNextType()
        {
            static size_t type_count = 0;
            return type_count++;
        }
    };

    template <typename EventType>
    class Event : BaseEvent
    {
    public:
        Event(const EventType& event) : event_(event) {}
        const EventType& _event;

        static size_t type()
        {
            static size_t _type = BaseEvent::getNextType();
            return _type;
        } 
    };

    class EventManager
    {
    public:
        template <class EventType>
        using callType = std::function<void(const EventType&)>;

        template <typename EventType>
        void subscribe(callType<EventType> callable)
        {
            size_t type = Event<EventType>::type();
            if (type >= _subscribers.size()){
                _subscribers.resize(type+1);
            }
            _subscribers.push_back(CallbackWrapper<EventType>(callable));
        }

        template <typename EventType>
        void dispatch(const EventType& event)
        {
            size_t type = Event<EventType>::type();
            if (type >= _subscribers.size()){
                _subscribers.resize(type+1);
            }

            Event<EventType> eventWrapper(event);
            for (auto& receiver : _subscribers[type]){
                receiver(eventWrapper);
            }
        }

        template <typename EventType>
        struct CallbackWrapper
        {
            CallbackWrapper(callType callable) : _callable(callable) {}
            void operator() (const BaseEvent& event) {
                _callable(static_cast<const Event<EventType>&(event).event_);
            }

            callType<EventType _callable;
        } ;

    private:
        std::vector<std::vector<callType<BaseEvent>>> _subscribers;
    };
}