#pragma once

#include "sgfevent.h"

namespace SGF
{

class EventKeyboard : public Event
{
public:
	enum class KeyCode
	{
		KEY_NONE,
		KEY_ESCAPE,
		KEY_SPACE,
		KEY_UP,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_DOWN
	};

	EventKeyboard(KeyCode keyCode, bool isPressed);

private:
	KeyCode _keyCode;
	bool _isPressed;

	friend class EventListenerKeyboard;
};

}
