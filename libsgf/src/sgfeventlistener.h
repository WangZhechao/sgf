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
		UNKNOWN,		//δ֪�¼�
		KEYBOARD,		//�����¼�
		MOUSE,			//����¼�
		WINDOW,			//�����¼�
		CUSTOM			//�Զ����¼�
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