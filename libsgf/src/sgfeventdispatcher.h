#pragma once

#include "sgfeventlistener.h"
#include <unordered_map>

namespace SGF
{

class Event;

class EventDispatcher
{
public:
	void addEventListener(EventListener* listener);
	void removeEventListener(EventListener* listener);
	void removeAllEventListeners();

	void dispatchEvent(Event* event);

	void setEnabled(bool enabled) { _isEnabled = enabled; }
	bool isEnabled() const { return _isEnabled; }

public:
	EventDispatcher();
	~EventDispatcher();

private:
	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;
	
	
	void dispatchEventToListener(EventListener* listener, const std::function<bool(EventListener*)>& onEvent);

	
	bool _isEnabled;
	std::unordered_map<EventListener::ListenerID, EventListener*> _listenerMap;
};

}

