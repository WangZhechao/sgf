#pragma once

#include "sgfevent.h"

namespace SGF
{

class EventMouse : public Event
{
public:
	enum class MouseEventType
	{
		MOUSE_NONE,
		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_CLICK,
		MOUSE_DBCLICK,
		MOUSE_MOVE
	};


	enum class MouseButton
	{
		BUTTON_UNSET = -1,
		BUTTON_LEFT = 0,
		BUTTON_RIGHT = 1,
		BUTTON_MIDDLE = 2
	};


public:
	EventMouse(MouseEventType mouseEventCode);


public:
	void setMouseButton(MouseButton button) { _mouseButton = button; }
	MouseButton getMouseButton() const { return _mouseButton; }
	int getCursorX() const { return _x; }
	int getCursorY() const { return _y; }


private:
	MouseEventType _mouseEventType;
	MouseButton _mouseButton;
	int _x;
	int _y;


	friend class EventListenerMouse;
};




}
