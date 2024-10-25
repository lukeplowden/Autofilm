#pragma once

#include "Core/Core.h"
#include "Events/Event.h"
// #include "Core/KeyCodes.h"
#include <sstream>

namespace Autofilm
{

	class KeyEvent : public Event
	{
	public:
		int GetKeyCode() const { return _keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(const int keycode, unsigned int ID)
			: _keyCode(keycode), _ID(ID) {}

		int _keyCode;
		unsigned int _ID;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const int keycode, bool isRepeat, unsigned int ID)
			: KeyEvent(keycode, ID), _IsRepeat(isRepeat) {}

		bool isRepeat() const { return _IsRepeat; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << _keyCode << " (repeat = " << _IsRepeat << ")" << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool _IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const int keycode, unsigned int ID)
			: KeyEvent(keycode, ID) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << _keyCode << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const int keycode, unsigned int ID)
			: KeyEvent(keycode, ID) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << _keyCode << " on Window " << _ID;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}