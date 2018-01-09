
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_H_INCLUDED_
#define _NGX_EVENT_H_INCLUDED_


#include "ngx_config.h"
#include "ngx_rbtree.h"

#define ngx_add_timer        ngx_event_add_timer
#define ngx_del_timer        ngx_event_del_timer

#define NGX_INVALID_INDEX  0xd0d0d0d0

typedef struct ngx_event_s   ngx_event_t;
typedef void(*ngx_event_handler_pt)(ngx_event_t *ev);

struct ngx_event_s {
    void            *data;

    unsigned         timedout:1;
    unsigned         timer_set:1;

	ngx_msec_t		 cycle;

    ngx_event_handler_pt  handler;

    ngx_rbtree_node_t   timer;
};


#endif /* _NGX_EVENT_H_INCLUDED_ */
