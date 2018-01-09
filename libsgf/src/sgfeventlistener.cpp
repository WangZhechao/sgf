#include "sgfeventlistener.h"


namespace SGF
{


EventListener::EventListener()
{}

EventListener::~EventListener()
{

}


bool EventListener::init(Type t, const ListenerID& listenerID, 
	const std::function<void(Event*)>& callback)

{
	_onEvent = callback;
	_type = t;
	_listenerID = listenerID;
	_isEnabled = true;

	return true;
}



}