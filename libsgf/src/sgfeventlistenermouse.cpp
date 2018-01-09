#include "sgfeventlistenermouse.h"

namespace SGF
{

const std::string EventListenerMouse::LISTENER_ID = "__sgf_mouse";

EventListenerMouse* EventListenerMouse::create()
{
	static EventListenerMouse keybord;
	return &keybord;
}


EventListenerMouse::EventListenerMouse()
	: onMouseDown(nullptr),
	onMouseUp(nullptr),
	onMouseMove(nullptr),
	onMouseClick(nullptr),
	onMouseDbClick(nullptr)
{
	init();
}


bool EventListenerMouse::init()
{
	auto listener = [this](Event* event) {
		auto mouseEvent = static_cast<EventMouse*>(event);
		switch (mouseEvent->_mouseEventType)
		{
		case EventMouse::MouseEventType::MOUSE_DOWN:
			if (onMouseDown != nullptr)
				onMouseDown(mouseEvent);
			break;
		case EventMouse::MouseEventType::MOUSE_UP:
			if (onMouseUp != nullptr)
				onMouseUp(mouseEvent);
			break;
		case EventMouse::MouseEventType::MOUSE_MOVE:
			if (onMouseMove != nullptr)
				onMouseMove(mouseEvent);
			break;
		case EventMouse::MouseEventType::MOUSE_CLICK:
			if (onMouseClick != nullptr)
				onMouseClick(mouseEvent);
			break;
		case EventMouse::MouseEventType::MOUSE_DBCLICK:
			if (onMouseDbClick != nullptr)
				onMouseDbClick(mouseEvent);
			break;
		default:
			break;
		}
	
	};


	if (EventListener::init(Type::MOUSE, LISTENER_ID, listener))
	{
		return true;
	}

	return false;
}


}