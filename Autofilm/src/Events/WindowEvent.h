#pragma once

#include "Events/Event.h"
#include <sstream>

namespace Autofilm 
{

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height, unsigned int ID)
			: _width(width), _height(height), _ID(ID) {}

		unsigned int GetWidth() const { return _width; }
		unsigned int GetHeight() const { return _height; }
		unsigned int getID() const { return _ID; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << _width << ", " << _height  << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int _width, _height;
		unsigned int _ID;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent(const unsigned int ID)
            : _ID(ID) {}

		unsigned int getID() const { return _ID; }
		
        std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent: " << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	
    private:
        unsigned int _ID;
    };

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}