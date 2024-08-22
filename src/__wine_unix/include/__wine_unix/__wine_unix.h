#pragma once

#include <stdint.h>
#include <limits.h>
#include <stddef.h>

#if defined(_WIN32) && !defined(__WINE__) || defined(__CYGWIN__)
#define __WINE_UNIX_DLLEXPORT __declspec(dllexport)
#define __WINE_UNIX_DEFAULTCALL __fastcall
#else
#define __WINE_UNIX_DLLEXPORT
#define __WINE_UNIX_DEFAULTCALL
#endif

#ifdef __cplusplus
#if __cplusplus <= 201107
#define __WINE_UNIX_NOEXCEPT throw()
#else
#define __WINE_UNIX_NOEXCEPT noexcept
#endif
extern "C"
{
#else
#define __WINE_UNIX_NOEXCEPT
#endif
#if INTPTR_MAX < INT_LEAST32_MAX
	typedef int_least32_t __wine_host_fd_t;
	typedef uint_least32_t __wine_errno_t;
#else
typedef ptrdiff_t __wine_host_fd_t;
typedef size_t __wine_errno_t;
#endif

	typedef __wine_host_fd_t __wine_host_errno_t;
	typedef __wine_host_fd_t __wine_host_flags_t;
	typedef __wine_host_fd_t __wine_host_mode_t;

	typedef struct
	{
		void *iov_base;
		size_t iov_len;
	} __wine_unix_iovec_t;

	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_host_errno_to_wine_errno(__wine_host_errno_t) __WINE_UNIX_NOEXCEPT;
	__WINE_UNIX_DLLEXPORT __wine_host_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_wine_errno_to_host_errno(__wine_errno_t) __WINE_UNIX_NOEXCEPT;

	typedef struct
	{
		__wine_errno_t errcode;
		size_t total;
		size_t baseindex;
		size_t index;
	} __wine_unix_result_ret_readwritev;

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_writev(__wine_host_fd_t, __wine_unix_iovec_t const *, size_t) __WINE_UNIX_NOEXCEPT;

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_readv(__wine_host_fd_t, __wine_unix_iovec_t *, size_t) __WINE_UNIX_NOEXCEPT;

	typedef struct
	{
		__wine_errno_t errcode;
		__wine_host_fd_t host_fd;
	} __wine_unix_result_ret_openat;

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_openat __WINE_UNIX_DEFAULTCALL __wine_unix_ret_openat(__wine_host_fd_t, void const *, size_t, __wine_host_flags_t, __wine_host_mode_t) __WINE_UNIX_NOEXCEPT;
	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_close(__wine_host_fd_t) __WINE_UNIX_NOEXCEPT;

#ifdef __cplusplus
}
#endif
