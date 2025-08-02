#pragma once
#define ARGLESS_CORE_HPP

#define _ARGLESS_BEGIN namespace maslowian { namespace argless {
#define _ARGLESS_END } }
#define _ARGLESS maslowian::argless::

#define _ARGLESS_PRIVATE_BEGIN _ARGLESS_BEGIN namespace core {
#define _ARGLESS_PRIVATE_END _ARGLESS_END }
#define _ARGLESS_PRIVATE _ARGLESS core::

#define _ARGLESS_STATIC_ASSERT(expr, msg) static_assert(expr, msg)

#if defined(_DEBUG) || defined(DEBUG)
#define _ARGLESS_ASSERT(expr, msg) do { if (!(expr)) throw std::runtime_error(msg); } while (0)
#else
#define _ARGLESS_ASSERT(expr, msg)
#endif

#define _ARGLESS_CONVERT_CHAR(char_type, to_char_type, value) [](char_type c) constexpr -> to_char_type { return static_cast<to_char_type>(c); }(value)

#define _ARGLESS_CHAR(char_type, value) _ARGLESS_CONVERT_CHAR(char, char_type, value)

// TODO: make it constexpr
#define _ARGLESS_CONVERT_TEXT(char_type, to_char_type, value) [](std::basic_string_view<char_type> str) -> std::basic_string<to_char_type> \
{ \
	std::basic_string<to_char_type> convert_str(str.size(), _ARGLESS_CHAR(to_char_type, '\0')); \
	for (size_t i = 0; i < str.size(); i++) \
		convert_str[i] = _ARGLESS_CHAR(to_char_type, str[i]); \
	return convert_str; \
}(value)

#define _ARGLESS_TEXT(char_type, value) _ARGLESS_CONVERT_TEXT(char, char_type, value)

// C++ version detection

#ifdef _MSVC_LANG
	#define _ARGLESS_CPP_VERSION _MSVC_LANG
#else
	#define _ARGLESS_CPP_VERSION __cplusplus
#endif

#if _ARGLESS_CPP_VERSION >= 201103L
	#define _ARGLESS_HAS_CPP11 1
#else
	#define _ARGLESS_HAS_CPP11 0
#endif

#if _ARGLESS_CPP_VERSION >= 201402L
	#define _ARGLESS_HAS_CPP14 1
#else
	#define _ARGLESS_HAS_CPP14 0
#endif

#if _ARGLESS_CPP_VERSION >= 201703L
	#define _ARGLESS_HAS_CPP17 1
#else
	#define _ARGLESS_HAS_CPP17 0
#endif

#if _ARGLESS_CPP_VERSION >= 202002L
	#define _ARGLESS_HAS_CPP20 1
#else
	#define _ARGLESS_HAS_CPP20 0
#endif

#undef _ARGLESS_CPP_VERSION

// C++ version specific macros

#if _ARGLESS_HAS_CPP20
#define _ARGLESS_HAS_CHAR8_T 1
#else
#define _ARGLESS_HAS_CHAR8_T 0
#endif
