#pragma once

namespace SGF
{

class Event
{
public:
	enum class Type
	{
		KEYBOARD,
		MOUSE,
		WINDOWS,
		CUSTOM,
	};


public:
	Event(Type type);
	virtual ~Event();

public:
	Type getType() const { return _type; }


private:
	Type _type;


	friend class EventDispatcher;
};

}
