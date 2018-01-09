#include "sgfeventlistenerkeyboard.h"

namespace SGF
{

const std::string EventListenerKeyboard::LISTENER_ID = "__sgf_keyboard";


EventListenerKeyboard* EventListenerKeyboard::create()
{
	static EventListenerKeyboard keybord;
	return &keybord;
}


EventListenerKeyboard::EventListenerKeyboard()
	: onKeyPressed(nullptr)
	, onKeyReleased(nullptr)
{
	init();
}

bool EventListenerKeyboard::init()
{
	auto listener = [this](Event* event){
        auto keyboardEvent = static_cast<EventKeyboard*>(event);
        if (keyboardEvent->_isPressed)
        {
            if (onKeyPressed != nullptr)
                onKeyPressed(keyboardEvent->_keyCode, event);
        }
        else
        {
            if (onKeyReleased != nullptr)
                onKeyReleased(keyboardEvent->_keyCode, event);
        }
    };


	if (EventListener::init(Type::KEYBOARD, LISTENER_ID, listener))
	{
		return true;
	}

	return false;
}

}