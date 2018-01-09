#include "sgfeventcustom.h"

namespace SGF
{

EventCustom::EventCustom(const std::string& eventName)
	: Event(Type::CUSTOM)
	, _userData(nullptr)
	, _eventName(eventName)
{

}

}