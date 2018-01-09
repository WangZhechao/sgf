
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_WIN32_CONFIG_H_INCLUDED_
#define _NGX_WIN32_CONFIG_H_INCLUDED_


/*
 * we need to include <windows.h> explicitly before <winsock2.h> because
 * the warning 4201 is enabled in <windows.h>
 */
#include <windows.h>

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <time.h>      /* localtime(), strftime() */


#ifdef _MSC_VER

/* the end of the precompiled headers */
#pragma hdrstop

#pragma warning(default:4201)

/* disable some "-W4" level warnings */

/* 'type cast': from function pointer to data pointer */
#pragma warning(disable:4054)

/* 'type cast': from data pointer to function pointer */
#pragma warning(disable:4055)

/* 'function' : different 'const' qualifiers */
#pragma warning(disable:4090)

/* unreferenced formal parameter */
#pragma warning(disable:4100)

/* FD_SET() and FD_CLR(): conditional expression is constant */
#pragma warning(disable:4127)

/* conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(disable:4244)

/* conversion from 'size_t' to 'type', possible loss of data */
#pragma warning(disable:4267)

/* array is too small to include a terminating null character */
#pragma warning(disable:4295)

#endif




#define ngx_inline          __inline
#define ngx_cdecl           __cdecl


#ifdef _MSC_VER
typedef unsigned __int32    uint32_t;
typedef __int32             int32_t;
typedef unsigned __int16    uint16_t;
#define ngx_libc_cdecl      __cdecl

#elif defined __BORLANDC__
typedef unsigned __int32    uint32_t;
typedef __int32             int32_t;
typedef unsigned __int16    uint16_t;
#define ngx_libc_cdecl      __cdecl

#else /* __WATCOMC__ */
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short int  uint16_t;
#define ngx_libc_cdecl

#endif

typedef __int64             int64_t;
typedef unsigned __int64    uint64_t;


#ifndef __GNUC__
#ifdef _WIN64
typedef __int64             ssize_t;
#else
typedef int                 ssize_t;
#endif
#endif

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

#ifdef _WIN64

#define NGX_PTR_SIZE            8
#define NGX_SIZE_T_LEN          (sizeof("-9223372036854775808") - 1)
#define NGX_MAX_SIZE_T_VALUE    9223372036854775807
#define NGX_TIME_T_LEN          (sizeof("-9223372036854775808") - 1)
#define NGX_TIME_T_SIZE         8
#define NGX_MAX_TIME_T_VALUE    9223372036854775807

#else

#define NGX_PTR_SIZE            4
#define NGX_SIZE_T_LEN          (sizeof("-2147483648") - 1)
#define NGX_MAX_SIZE_T_VALUE    2147483647
#define NGX_TIME_T_LEN          (sizeof("-2147483648") - 1)
#define NGX_TIME_T_SIZE         4
#define NGX_MAX_TIME_T_VALUE    2147483647

#endif


#define NGX_OFF_T_LEN           (sizeof("-9223372036854775807") - 1)
#define NGX_MAX_OFF_T_VALUE     9223372036854775807
#define NGX_SIG_ATOMIC_T_SIZE   4


#define NGX_HAVE_LITTLE_ENDIAN  1
#define NGX_HAVE_NONALIGNED     1


#define NGX_WIN_NT        200000


#define NGX_LISTEN_BACKLOG           511


#ifndef NGX_HAVE_INHERITED_NONBLOCK
#define NGX_HAVE_INHERITED_NONBLOCK  1
#endif

#ifndef NGX_HAVE_CASELESS_FILESYSTEM
#define NGX_HAVE_CASELESS_FILESYSTEM  1
#endif

#ifndef NGX_HAVE_WIN32_TRANSMITPACKETS
#define NGX_HAVE_WIN32_TRANSMITPACKETS  1
#define NGX_HAVE_WIN32_TRANSMITFILE     0
#endif

#ifndef NGX_HAVE_WIN32_TRANSMITFILE
#define NGX_HAVE_WIN32_TRANSMITFILE  1
#endif

#if (NGX_HAVE_WIN32_TRANSMITPACKETS) || (NGX_HAVE_WIN32_TRANSMITFILE)
#define NGX_HAVE_SENDFILE  1
#endif

#ifndef NGX_HAVE_SO_SNDLOWAT
/* setsockopt(SO_SNDLOWAT) returns error WSAENOPROTOOPT */
#define NGX_HAVE_SO_SNDLOWAT         0
#endif

#define NGX_HAVE_GETADDRINFO         1

#define ngx_random               rand
#define ngx_debug_init()


#endif /* _NGX_WIN32_CONFIG_H_INCLUDED_ */
