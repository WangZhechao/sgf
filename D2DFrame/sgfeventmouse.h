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


	enum MouseButton
	{
		BUTTON_UNSET = 0,
		BUTTON_LEFT = 1,
		BUTTON_RIGHT = 2,
		BUTTON_MIDDLE = 4
	};


public:
	EventMouse(MouseEventType mouseEventCode);
	void setCursorPosition(int x, int y);

public:
	void setMouseButton(int buttons) { _mouseButton = buttons; }
	int getMouseButton() { return _mouseButton; }

	int getCursorX() const { return _x; }
	int getCursorY() const { return _y; }


private:
	MouseEventType _mouseEventType;
	int _mouseButton;
	int _x;
	int _y;


	friend class EventListenerMouse;
};




}
