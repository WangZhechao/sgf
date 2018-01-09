
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_TIMER_H_INCLUDED_
#define _NGX_EVENT_TIMER_H_INCLUDED_



#include "ngx_time.h"
#include "ngx_event.h"
#include "ngx_times.h"

#define NGX_TIMER_INFINITE  (ngx_msec_t) -1

#define NGX_TIMER_LAZY_DELAY  300

extern ngx_rbtree_t  ngx_event_timer_rbtree;


EXTERN_C ngx_int_t ngx_event_timer_init();
EXTERN_C ngx_msec_t ngx_event_find_timer(void);
EXTERN_C void ngx_event_expire_timers(void);
EXTERN_C void ngx_event_del_timer(ngx_event_t *ev);
EXTERN_C void ngx_event_add_timer(ngx_event_t *ev, ngx_msec_t timer);




#endif /* _NGX_EVENT_TIMER_H_INCLUDED_ */
