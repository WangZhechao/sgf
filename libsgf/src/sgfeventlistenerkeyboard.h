#pragma once

#include "sgfeventlistener.h"
#include "sgfeventkeyboard.h"

namespace SGF
{
class Event;


class EventListenerKeyboard : public EventListener
{
public:
	static const std::string LISTENER_ID;

	static EventListenerKeyboard* create();

	std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
	std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;

private:
	EventListenerKeyboard();
	EventListenerKeyboard(const EventListenerKeyboard &) = delete;
	EventListenerKeyboard &operator = (const EventListenerKeyboard &) = delete;

	bool init();
};



}