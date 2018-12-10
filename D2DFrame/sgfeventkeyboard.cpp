#include "sgfeventkeyboard.h"

namespace SGF
{

EventKeyboard::EventKeyboard(KeyCode keyCode, bool isPressed)
	: Event(Type::KEYBOARD)
	, _keyCode(keyCode)
	, _isPressed(isPressed)
{
	
}

}