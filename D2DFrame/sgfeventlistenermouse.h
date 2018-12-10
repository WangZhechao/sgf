#pragma once

#include "sgfeventlistener.h"
#include "sgfeventmouse.h"

namespace SGF
{

class EventListenerMouse : public EventListener
{
public:
	static const std::string LISTENER_ID;

	static EventListenerMouse* create();

	std::function<void(EventMouse* event)> onMouseDown;
	std::function<void(EventMouse* event)> onMouseUp;
	std::function<void(EventMouse* event)> onMouseMove;
	std::function<void(EventMouse* event)> onMouseClick;
	std::function<void(EventMouse* event)> onMouseDbClick;

private:
	EventListenerMouse();
	EventListenerMouse(const EventListenerMouse &) = delete;
	EventListenerMouse &operator = (const EventListenerMouse &) = delete;

	bool init();
};

}
