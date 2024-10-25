#pragma once

#include "Events/Event.h"
#include <sstream>

namespace Autofilm 
{

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y, unsigned int ID)
			: _MouseX(x), _MouseY(y), _ID(ID) {}

		float GetX() const { return _MouseX; }
		float GetY() const { return _MouseY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << _MouseX << ", " << _MouseY  << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float _MouseX, _MouseY;
		unsigned int _ID;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset, unsigned int ID)
			: _xOffset(xOffset), _yOffset(yOffset) {}

		float GetXOffset() const { return _xOffset; }
		float GetYOffset() const { return _yOffset; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset() << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float _xOffset, _yOffset;
		unsigned int _ID;
	};

	class MouseButtonEvent : public Event
	{
	public:
		int GetMouseButton() const { return _Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(const int button, unsigned int ID)
			: _Button(button), _ID(ID) {}

		int _Button;
		unsigned int _ID;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const int button, unsigned int ID)
			: MouseButtonEvent(button, ID) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << _Button << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const int button, unsigned int ID)
			: MouseButtonEvent(button, ID) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << _Button << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}