#include "sgfeventmouse.h"

namespace SGF
{

EventMouse::EventMouse(MouseEventType mouseEventCode)
	: Event(Type::MOUSE)
	, _mouseEventType(mouseEventCode)
	, _mouseButton(MouseButton::BUTTON_UNSET)
	, _x(0)
	, _y(0)
{
	
}



}