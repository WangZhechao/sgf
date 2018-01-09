#pragma once

#include "sgfevent.h"
#include <string>

namespace SGF
{

class EventCustom : public Event
{
public:
	EventCustom(const std::string& eventName);

	void setUserData(void* data) { _userData = data; }
	void* getUserData() const { return _userData; }

	const std::string& getEventName() const { return _eventName; }

private:
	void *_userData;
	std::string _eventName;

	friend class EventListenerCustom;
};


}
