#pragma once

#include "sgfeventlistener.h"
#include "sgfeventcustom.h"
#include <memory>

namespace SGF
{

class EventListenerCustom : public EventListener
{
public:
	//��ʱ����������Ҫ�ֶ�����
	static EventListenerCustom* create(const std::string& eventName, const std::function<void(EventCustom*)>& callback);

	EventListenerCustom();
	~EventListenerCustom();


private:
	EventListenerCustom(const EventListenerCustom &) = delete;
	EventListenerCustom &operator = (const EventListenerCustom &) = delete;


public:
	bool init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback);

	std::function<void(EventCustom*)> _onCustomEvent;
};


}
