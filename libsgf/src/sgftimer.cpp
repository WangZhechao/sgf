#include "sgftimer.h"

#include "ngx_event_timer.h"

namespace SGF
{
static float g_update_tick = 0.0f;


bool Timer::init()
{
	ngx_time_init();
	ngx_event_timer_init();

	return true;
}


void Timer::unInit()
{

}


void Timer::update(float t)
{
	g_update_tick += t;

	ngx_time_update(g_update_tick);
	ngx_event_expire_timers();
}


void Timer::add(Timer *ev)
{
	if(ev)
		ngx_add_timer(ev, ev->cycle);
}


void Timer::del(Timer *ev)
{
	if (ev)
		ngx_del_timer(ev);
}


}