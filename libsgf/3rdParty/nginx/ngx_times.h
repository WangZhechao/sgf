
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_TIMES_H_INCLUDED_
#define _NGX_TIMES_H_INCLUDED_


#include "ngx_time.h"


 /*
 * milliseconds elapsed since epoch and truncated to ngx_msec_t,
 * used in event timers
 */
extern volatile ngx_msec_t  ngx_current_msec;

//typedef struct {
//    time_t      sec;
//    ngx_uint_t  msec;
//    ngx_int_t   gmtoff;
//} ngx_time_t;


EXTERN_C void ngx_time_init();
EXTERN_C void ngx_time_update(ngx_msec_t t);



//void ngx_time_sigsafe_update(void);
//u_char *ngx_http_time(u_char *buf, time_t t);
//u_char *ngx_http_cookie_time(u_char *buf, time_t t);
//void ngx_gmtime(time_t t, ngx_tm_t *tp);
//
//time_t ngx_next_time(time_t when);
//#define ngx_next_time_n      "mktime()"
//
//
//extern volatile ngx_time_t  *ngx_cached_time;
//
//#define ngx_time()           ngx_cached_time->sec
//#define ngx_timeofday()      (ngx_time_t *) ngx_cached_time




#endif /* _NGX_TIMES_H_INCLUDED_ */
