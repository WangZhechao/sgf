#pragma once

#include "ngx_event_timer.h"

namespace SGF
{

namespace Timer
{
	typedef ngx_event_t Timer;

	bool init();
	void unInit();
	void update(float t);
	void add(Timer *ev);
	void del(Timer *ev);
}

}