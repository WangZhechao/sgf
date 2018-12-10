#include "sgfeventlistenercustom.h"

namespace SGF
{

EventListenerCustom::EventListenerCustom()
	: _onCustomEvent(nullptr)
{

}


EventListenerCustom::~EventListenerCustom()
{
}


EventListenerCustom* EventListenerCustom::create(const std::string& eventName, const std::function<void(EventCustom*)>& callback)
{
	EventListenerCustom* ret = new (std::nothrow) EventListenerCustom();
	if (ret && ret->init(eventName, callback))
	{
		return ret;
	}

	return nullptr;
}


void EventListenerCustom::autoDestory()
{
	delete this;
}


bool EventListenerCustom::init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback)
{
	bool ret = false;

	_onCustomEvent = callback;

	auto listener = [this](Event* event) {
		if (_onCustomEvent != nullptr)
		{
			_onCustomEvent(static_cast<EventCustom*>(event));
		}
	};

	if (EventListener::init(EventListener::Type::CUSTOM, listenerId, listener))
	{
		ret = true;
	}

	return ret;
}


}