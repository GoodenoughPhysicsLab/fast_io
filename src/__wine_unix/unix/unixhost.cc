﻿#include <__wine_unix/__wine_unix.h>
#include <__wine_unix/__wine_unix_errno.h>
#include <errno.h>
#include <limits>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

namespace __wine_unix
{

inline bool sanitize_file_path(void const *str, size_t len) noexcept
{
	char const *cstr{static_cast<char const *>(str)};
	if (__builtin_strnlen(cstr, len) == len)
	{
		return cstr[len] == 0;
	}
	return false;
}
inline constexpr ::__wine_errno_t host_errno_to_wine_errno(int val) noexcept
{
	/*
	Todo. just implement a place holder first so i can test whether this works out
	*/
	// #ifdef __linux__
	return static_cast<::__wine_errno_t>(static_cast<unsigned>(val));
	// #endif
}

template <typename T>
inline constexpr bool value_out_of_int_range(T val) noexcept
{
	constexpr auto intmn{::std::numeric_limits<int>::min()};
	constexpr auto intmx{::std::numeric_limits<int>::max()};
	return val < intmn || intmx < val;
}

} // namespace __wine_unix

extern "C"
{

	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_host_errno_to_wine_errno(__wine_host_errno_t val) __WINE_UNIX_NOEXCEPT
	{
		if (::__wine_unix::value_out_of_int_range(val))
		{
			return __WINE_UNIX_ERRNO_EINVAL;
		}
		return __wine_unix::host_errno_to_wine_errno(static_cast<int>(val));
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_openat __WINE_UNIX_DEFAULTCALL __wine_unix_ret_openat(__wine_host_fd_t dirfd, void const *filename, size_t filenamelen, __wine_host_flags_t flags, __wine_host_mode_t mode) __WINE_UNIX_NOEXCEPT
	{
		if (::__wine_unix::value_out_of_int_range(dirfd))
		{
			return {__WINE_UNIX_ERRNO_EINVAL, -1};
		}
		if (!::__wine_unix::sanitize_file_path(filename, filenamelen))
		{
			return {__WINE_UNIX_ERRNO_EINVAL, -1};
		}
		int fd = ::openat(dirfd, reinterpret_cast<char const *>(filename), flags, mode);
		if (fd == -1)
		{
			return {::__wine_unix::host_errno_to_wine_errno(errno), -1};
		}
		return {0, static_cast<__wine_host_fd_t>(fd)};
	}

	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_close(__wine_host_fd_t fd) __WINE_UNIX_NOEXCEPT
	{
		if (::__wine_unix::value_out_of_int_range(fd))
		{
			return __WINE_UNIX_ERRNO_EINVAL;
		}
		if (::close(fd) == -1)
		{
			return __wine_unix::host_errno_to_wine_errno(errno);
		}
		return __WINE_UNIX_ERRNO_SUCCESS;
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_writev(__wine_host_fd_t fd, __wine_unix_iovec_t const *iovs, size_t iovsize) __WINE_UNIX_NOEXCEPT
	{
		if (::__wine_unix::value_out_of_int_range(fd))
		{
			return {__WINE_UNIX_ERRNO_EINVAL, 0, 0, 0};
		}
		auto ret = ::writev(static_cast<int>(fd), reinterpret_cast<struct iovec const *>(iovs), iovsize);
		if (ret == -1)
		{
			return {__wine_unix::host_errno_to_wine_errno(errno), 0, 0, 0};
		}
		return {0, static_cast<size_t>(ret), iovsize, 0};
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_readv(__wine_host_fd_t fd, __wine_unix_iovec_t *iovs, size_t iovsize) __WINE_UNIX_NOEXCEPT
	{
		if (::__wine_unix::value_out_of_int_range(fd))
		{
			return {__WINE_UNIX_ERRNO_EINVAL, 0, 0, 0};
		}
		auto ret = ::readv(static_cast<int>(fd), reinterpret_cast<struct iovec *>(iovs), iovsize);
		if (ret == -1)
		{
			return {__wine_unix::host_errno_to_wine_errno(errno), 0, 0, 0};
		}
		return {0, static_cast<size_t>(ret), iovsize, 0};
	}
}