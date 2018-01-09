
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include "ngx_event_timer.h"


ngx_rbtree_t              ngx_event_timer_rbtree;
static ngx_rbtree_node_t  ngx_event_timer_sentinel;

/*
 * the event timer rbtree may contain the duplicate keys, however,
 * it should not be a problem, because we use the rbtree to find
 * a minimum timer value only
 */

EXTERN_C ngx_int_t
ngx_event_timer_init()
{
    ngx_rbtree_init(&ngx_event_timer_rbtree, &ngx_event_timer_sentinel,
                    ngx_rbtree_insert_timer_value);

    return NGX_OK;
}


EXTERN_C ngx_msec_t
ngx_event_find_timer(void)
{
    ngx_msec_int_t      timer;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    if (ngx_event_timer_rbtree.root == &ngx_event_timer_sentinel) {
        return NGX_TIMER_INFINITE;
    }

    root = ngx_event_timer_rbtree.root;
    sentinel = ngx_event_timer_rbtree.sentinel;

    node = ngx_rbtree_min(root, sentinel);

    timer = (ngx_msec_int_t) (node->key - ngx_current_msec);

    return (ngx_msec_t) (timer > 0 ? timer : 0);
}


EXTERN_C void
ngx_event_expire_timers(void)
{
    ngx_event_t        *ev;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    sentinel = ngx_event_timer_rbtree.sentinel;

    for ( ;; ) {
        root = ngx_event_timer_rbtree.root;

        if (root == sentinel) {
            return;
        }

        node = ngx_rbtree_min(root, sentinel);

        /* node->key > ngx_current_msec */

        if ((ngx_msec_int_t) (node->key - ngx_current_msec) > 0) {
            return;
        }

        ev = (ngx_event_t *) ((char *) node - offsetof(ngx_event_t, timer));


        ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);

#if (NGX_DEBUG)
        ev->timer.left = NULL;
        ev->timer.right = NULL;
        ev->timer.parent = NULL;
#endif

        ev->timer_set = 0;

        ev->timedout = 1;

        ev->handler(ev);
    }
}


EXTERN_C void ngx_event_del_timer(ngx_event_t *ev)
{
	ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);

#if (NGX_DEBUG)
	ev->timer.left = NULL;
	ev->timer.right = NULL;
	ev->timer.parent = NULL;
#endif

	ev->timer_set = 0;
}


EXTERN_C void ngx_event_add_timer(ngx_event_t *ev, ngx_msec_t timer)
{
	ngx_msec_t      key;
	ngx_msec_int_t  diff;

	key = ngx_current_msec + timer;

	if (ev->timer_set) {

		/*
		* Use a previous timer value if difference between it and a new
		* value is less than NGX_TIMER_LAZY_DELAY milliseconds: this allows
		* to minimize the rbtree operations for fast connections.
		*/

		diff = (ngx_msec_int_t)(key - ev->timer.key);

		if (ngx_abs(diff) < NGX_TIMER_LAZY_DELAY) {
			return;
		}

		ngx_del_timer(ev);
	}

	ev->timer.key = key;

	ngx_rbtree_insert(&ngx_event_timer_rbtree, &ev->timer);

	ev->timer_set = 1;
}
