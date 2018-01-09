#pragma once

#include <functional>
#include <string>
#include "sgfevent.h"

class Event;

namespace SGF
{

class EventListener
{
public:
	enum class Type
	{
		UNKNOWN,		//未知事件
		KEYBOARD,		//键盘事件
		MOUSE,			//鼠标事件
		WINDOW,			//窗口事件
		CUSTOM			//自定义事件
	};


	typedef std::string ListenerID;


public:
	EventListener();
	virtual ~EventListener();


public:
	bool init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback);
	void setEnabled(bool enabled) { _isEnabled = enabled; }
	bool isEnabled() const { return _isEnabled; }
	const ListenerID& getListenerID() const { return _listenerID; }

private:
	bool _isEnabled;
	Type _type;
	ListenerID _listenerID;
	std::function<void(Event*)> _onEvent;


	friend class EventDispatcher;
};



}