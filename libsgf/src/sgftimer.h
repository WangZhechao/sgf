#pragma once

#include "ngx_event_timer.h"

namespace SGF
{

namespace Timer
{
	typedef ngx_event_t timer;

	bool init();
	void unInit();
	void update(float t);
	void add(timer *ev);
	void del(timer *ev);
}

}