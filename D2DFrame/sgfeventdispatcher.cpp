#include "sgfeventdispatcher.h"
#include "sgfeventlistenerkeyboard.h"
#include "sgfeventlistenermouse.h"
#include "sgfeventlistenercustom.h"
#include <assert.h>

namespace SGF
{

static EventListener::ListenerID __getListenerID(Event* event)
{
	EventListener::ListenerID ret;
	switch (event->getType())
	{
	case Event::Type::CUSTOM:
	{
		auto customEvent = static_cast<EventCustom*>(event);
		ret = customEvent->getEventName();
	}
	break;

	case Event::Type::KEYBOARD:
		ret = EventListenerKeyboard::LISTENER_ID;
		break;
	case Event::Type::MOUSE:
		ret = EventListenerMouse::LISTENER_ID;
		break;

	default:
		assert(false && "Invalid type!");
		break;
	}

	return ret;
}

EventDispatcher::EventDispatcher()
	: _isEnabled(true)
{

}


EventDispatcher::~EventDispatcher()
{
	removeAllEventListeners();
}


void EventDispatcher::addEventListener(EventListener* listener)
{
	assert(listener);

	EventListener::ListenerID listenerID = listener->getListenerID();

	auto itr = _listenerMap.find(listenerID);

	//如果已经不存在该监听器
	if (itr == _listenerMap.end()) 
	{
		_listenerMap.emplace(listenerID, listener);
	}
	else
	{
		//需要销毁	
		itr->second->autoDestory();
		itr->second = listener;
	}
}


void EventDispatcher::removeEventListener(EventListener* listener)
{
	if (listener == nullptr)
		return;

	EventListener::ListenerID listenerID = listener->getListenerID();

	auto itr = _listenerMap.find(listenerID);
	if (itr != _listenerMap.end())
	{
		itr->second->autoDestory();
		_listenerMap.erase(itr);
	}
}


void EventDispatcher::removeAllEventListeners()
{
	for (auto itr = _listenerMap.begin(); itr != _listenerMap.end(); ++itr) {
		itr->second->autoDestory();
	}
	_listenerMap.clear();
}


void EventDispatcher::dispatchEvent(Event* event)
{
	if (!_isEnabled)
		return;

	auto listenerID = __getListenerID(event);

	//简化问题，事件存在唯一性，不存在同一事件多个监听
	auto pfnDispatchEventToListener = &EventDispatcher::dispatchEventToListener;

	auto itr = _listenerMap.find(listenerID);
	if (itr != _listenerMap.end())
	{
		auto listeners = itr->second;

		auto onEvent = [&event](EventListener* listener) -> bool{
			listener->_onEvent(event);
			return true;
		};

		(this->*pfnDispatchEventToListener)(listeners, onEvent);
	}
}


void EventDispatcher::dispatchEventToListener(EventListener* listener, 
	const std::function<bool(EventListener*)>& onEvent)
{
	if (listener->isEnabled())
		onEvent(listener);
}

}