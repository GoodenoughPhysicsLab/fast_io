﻿#pragma once

namespace fast_io
{
enum class win9x_at_flags : ::std::uint_least32_t
{
	eaccess = static_cast<::std::uint_least32_t>(1),
	symlink_nofollow = static_cast<::std::uint_least32_t>(1) << 1,
	no_automount = static_cast<::std::uint_least32_t>(1) << 2,
	removedir = static_cast<::std::uint_least32_t>(1) << 3,
	empty_path = static_cast<::std::uint_least32_t>(1) << 4
};

constexpr win9x_at_flags operator&(win9x_at_flags x, win9x_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<win9x_at_flags>::type;
	return static_cast<win9x_at_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr win9x_at_flags operator|(win9x_at_flags x, win9x_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<win9x_at_flags>::type;
	return static_cast<win9x_at_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr win9x_at_flags operator^(win9x_at_flags x, win9x_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<win9x_at_flags>::type;
	return static_cast<win9x_at_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr win9x_at_flags operator~(win9x_at_flags x) noexcept
{
	using utype = typename ::std::underlying_type<win9x_at_flags>::type;
	return static_cast<win9x_at_flags>(~static_cast<utype>(x));
}

inline constexpr win9x_at_flags &operator&=(win9x_at_flags &x, win9x_at_flags y) noexcept
{
	return x = x & y;
}

inline constexpr win9x_at_flags &operator|=(win9x_at_flags &x, win9x_at_flags y) noexcept
{
	return x = x | y;
}

inline constexpr win9x_at_flags &operator^=(win9x_at_flags &x, win9x_at_flags y) noexcept
{
	return x = x ^ y;
}

namespace win32::details
{

::fast_io::tlc::u8string concat_tlc_win9x_path_uncheck_whether_exist(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size) noexcept
{
	auto const beg{path_c_str};
	auto curr{beg};

	if (auto const fc{*curr++}; fc == u8'+' ||
								fc == u8'-' ||
								fc == u8'.') [[unlikely]]
	{
		throw_win32_error(3221225530);
	}

	for (; curr != beg + path_size; ++curr)
	{
		auto fc{*curr};
		if (fc == u8'/' ||
			fc == u8'\\' ||
			fc == u8'\t' ||
			fc == u8'\b' ||
			fc == u8'@' ||
			fc == u8'#' ||
			fc == u8'$' ||
			fc == u8'%' ||
			fc == u8'^' ||
			fc == u8'&' ||
			fc == u8'*' ||
			fc == u8'(' ||
			fc == u8')' ||
			fc == u8'[' ||
			fc == u8']') [[unlikely]]
		{
			throw_win32_error(3221225530);
		}
	}

	return ::fast_io::tlc::u8concat_fast_io_tlc(::fast_io::mnp::code_cvt(dirhd.path), u8"\\", ::fast_io::mnp::os_c_str_with_known_size(beg, path_size));
}

inline void win9x_unlinkat_impl(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, win9x_at_flags flags)
{
	auto file_or_path{concat_tlc_win9x_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	switch (flags)
	{
	case ::fast_io::win9x_at_flags::removedir:
		if (!::fast_io::win32::RemoveDirectoryA(reinterpret_cast<char const *>(file_or_path.c_str()))) [[unlikely]]
		{
			throw_win32_error();
		}
		break;
	default:
		if (!::fast_io::win32::DeleteFileA(reinterpret_cast<char const *>(file_or_path.c_str()))) [[unlikely]]
		{
			throw_win32_error();
		}
		break;
	}
}

inline void win9x_mkdirat_impl(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, perms pm)
{
	auto path{concat_tlc_win9x_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	if (!::fast_io::win32::CreateDirectoryA(reinterpret_cast<char const *>(path.c_str()), nullptr)) [[unlikely]]
	{
		throw_win32_error();
	}

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == -1) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	if ((pm & perms::owner_write) == perms::none)
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr | 0x00000001)) [[unlikely]]
		{
			throw_win32_error();
		}
	}
	else
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr & ~static_cast<::std::uint_least32_t>(0x00000001))) [[unlikely]]
		{
			throw_win32_error();
		}
	}
}

inline void win9x_faccessat_impl(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, access_how mode, [[maybe_unused]] win9x_at_flags flags)
{
	switch (mode)
	{
	case ::fast_io::access_how::f_ok:
		[[fallthrough]];
	case ::fast_io::access_how::x_ok:
		[[fallthrough]];
	case ::fast_io::access_how::w_ok:
		[[fallthrough]];
	case ::fast_io::access_how::r_ok:
		break;
	default:
		throw_win32_error(0x57);
	}

	auto path{concat_tlc_win9x_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == -1) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	switch (mode)
	{
	case ::fast_io::access_how::f_ok:
		return;
	case ::fast_io::access_how::x_ok:
		// Like mounted ntfs in wsl, all files and dirs are executable
		return;
	case ::fast_io::access_how::w_ok:
	{
		if ((attr & 0x00000001) == 0x00000001) [[unlikely]]
		{
			throw_win32_error(0x57); // no access
		}
		return;
	}
	case ::fast_io::access_how::r_ok:
		return;
	default:
		::fast_io::unreachable();
	}
}

inline void win9x_fchmodat_impl(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, perms pm, [[maybe_unused]] win9x_at_flags flags)
{
	auto path{concat_tlc_win9x_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == -1) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	if ((pm & perms::owner_write) == perms::none)
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr | 0x00000001))
		{
			throw_win32_error();
		}
	}
	else
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr & ~static_cast<::std::uint_least32_t>(0x00000001)))
		{
			throw_win32_error();
		}
	}
}

[[noreturn]] inline void win9x_fchownat_impl(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size,
											 [[maybe_unused]] ::std::uintmax_t owner, [[maybe_unused]] ::std::uintmax_t group, [[maybe_unused]] win9x_at_flags flags)
{
	auto path{concat_tlc_win9x_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == -1) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	// Windows does not use POSIX user group system. stub it and it is perfectly fine.
	// But nt_fchownat, zw_fchownat will not be provided since they do not exist.
	throw_win32_error(0x1);
}

inline posix_file_status win9x_fstatat_impl(::fast_io::win9x_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, [[maybe_unused]] win9x_at_flags flags)
{
	auto path{concat_tlc_win9x_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == -1) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	::fast_io::file_type ft{::fast_io::file_type::directory};

	if ((attr & 0x10) == 0x10)
	{
		ft = ::fast_io::file_type::regular;
	}

	return posix_file_status{0,
							 0,
							 static_cast<perms>(attr),
							 ft,
							 0,
							 0,
							 0,
							 0,
							 0,
							 131072,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0};
}

template <::fast_io::details::posix_api_22 dsp, typename... Args>
inline auto win9x_22_api_dispatcher(::fast_io::win9x_dir_handle const &olddirhd, char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
									::fast_io::win9x_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_22::renameat)
	{
		// win9x_renameat_impl(olddirhd, oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_22::linkat)
	{
		// win9x_linkat_impl(olddirhd, oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
}

template <::fast_io::details::posix_api_12 dsp, typename... Args>
inline auto win9x_12_api_dispatcher(char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
									::fast_io::win9x_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size)
{
	if constexpr (dsp == ::fast_io::details::posix_api_12::symlinkat)
	{
		// win9x_symlinkat_impl(oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size);
	}
}

template <::fast_io::details::posix_api_1x dsp, typename... Args>
inline auto win9x_1x_api_dispatcher(::fast_io::win9x_dir_handle const &dir_handle, char8_t const *path_c_str, ::std::size_t path_size, Args... args)
{

	if constexpr (dsp == ::fast_io::details::posix_api_1x::faccessat)
	{
		win9x_faccessat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchmodat)
	{
		win9x_fchmodat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchownat)
	{
		win9x_fchownat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fstatat)
	{
		// return win9x_fstatat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::mkdirat)
	{
		win9x_mkdirat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::unlinkat)
	{
		win9x_unlinkat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::utimensat)
	{
		// win9x_utimensat_impl(dir_handle, path_c_str, path_size, args...);
	}
}

template <::fast_io::details::posix_api_1x dsp, typename path_type, typename... Args>
inline auto win9x_deal_with1x(::fast_io::win9x_dir_handle const &dir_handle, path_type const &path, Args... args)
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	return win32_api_common_9xa(
		path, [&](char const *path_c_str, ::std::size_t path_size) { return win9x_1x_api_dispatcher<dsp>(dir_handle, reinterpret_cast<char8_t_const_may_alias_ptr>(path_c_str), path_size, args...); });
}

template <::fast_io::details::posix_api_12 dsp, ::fast_io::constructible_to_os_c_str old_path_type,
		  ::fast_io::constructible_to_os_c_str new_path_type>
inline auto win9x_deal_with12(old_path_type const &oldpath, ::fast_io::win9x_dir_handle const &newdirfd, new_path_type const &newpath)
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	return win32_api_common_9xa(
		oldpath,
		[&](char const *oldpath_c_str, ::std::size_t oldpath_size) {
			return win32_api_common_9xa(
				newpath, [&](char const *newpath_c_str, ::std::size_t newpath_size) { return win9x_12_api_dispatcher<dsp>(reinterpret_cast<char8_t_const_may_alias_ptr>(oldpath_c_str), oldpath_size, newdirfd, reinterpret_cast<char8_t_const_may_alias_ptr>(newpath_c_str), newpath_size); });
		});
}

template <::fast_io::details::posix_api_22 dsp, typename oldpath_type, typename newpath_type, typename... Args>
inline auto win9x_deal_with22(::fast_io::win9x_dir_handle const &olddirhd, oldpath_type const &oldpath, ::fast_io::win9x_dir_handle const &newdirhd, newpath_type const &newpath, Args... args)
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	return win32_api_common_9xa(oldpath,
								[&](char const *oldpath_c_str, ::std::size_t oldpath_size) {
									return win32_api_common_9xa(newpath,
																[&](char const *newpath_c_str, ::std::size_t newpath_size) {
																	return win9x_22_api_dispatcher<dsp>(olddirhd, reinterpret_cast<char8_t_const_may_alias_ptr>(oldpath_c_str), oldpath_size, newdirhd,
																										reinterpret_cast<char8_t_const_may_alias_ptr>(newpath_c_str), newpath_size, args...);
																});
								});
}

} // namespace win32::details

using native_at_flags = win9x_at_flags;

template <::fast_io::constructible_to_os_c_str path_type>
inline void win9x_unlinkat(::fast_io::win9x_at_entry const &ent, path_type &&path, win9x_at_flags flags = {})
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::unlinkat>(ent.handle, path, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_unlinkat(::fast_io::win9x_at_entry const &ent, path_type &&path, win9x_at_flags flags = {})
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::unlinkat>(ent.handle, path, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win9x_mkdirat(::fast_io::win9x_at_entry const &ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::mkdirat>(ent.handle, path, pm);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_mkdirat(::fast_io::win9x_at_entry const &ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::mkdirat>(ent.handle, path, pm);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win9x_faccessat(::fast_io::win9x_at_entry const &ent, path_type const &path, access_how mode, win9x_at_flags flags = win9x_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::faccessat>(ent.handle, path, mode, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_faccessat(::fast_io::win9x_at_entry const &ent, path_type const &path, access_how mode, win9x_at_flags flags = win9x_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::faccessat>(ent.handle, path, mode, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win9x_fchmodat(::fast_io::win9x_at_entry const &ent, path_type const &path, perms mode, win9x_at_flags flags = win9x_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchmodat(::fast_io::win9x_at_entry const &ent, path_type const &path, perms mode, win9x_at_flags flags = win9x_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win9x_fchownat(::fast_io::win9x_at_entry const &ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, win9x_at_flags flags = win9x_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchownat(::fast_io::win9x_at_entry const &ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, win9x_at_flags flags = win9x_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win9x_deal_with1x<details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags);
}
} // namespace fast_io
