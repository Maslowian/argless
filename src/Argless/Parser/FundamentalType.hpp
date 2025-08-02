#pragma once
#include <algorithm>
#include <locale>
#include "../StringToType.hpp"
#include "../Args.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class FundamentalType
{
	template <typename T>
	struct is_character
	{
		static constexpr inline bool value = std::is_same_v<T, char> || std::is_same_v<T, wchar_t> ||
#if _ARGLESS_HAS_CHAR8_T
			std::is_same_v<T, char8_t> ||
#endif
			std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;
	};
	template <typename T>
	struct is_integer
	{
		static constexpr inline bool value = std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, short> || std::is_same_v<T, unsigned short> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_same_v<T, long> || std::is_same_v<T, unsigned long> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>;
	};
	template <typename T>
	struct is_floating_point
	{
		static constexpr inline bool value = std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = std::is_same_v<T, bool> || is_character<T>::value || is_integer<T>::value || is_floating_point<T>::value;
	};

public:	
	template <typename CharT, typename T, std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Boolean;
		auto& value = *arg.value();

		std::basic_string<CharT> value_lower(value.size(), 0);
		std::locale loc;
		std::transform(value.begin(), value.end(), value_lower.begin(), [&loc](CharT c) { return std::tolower(c, loc); });

		bool result;

		if (value_lower == _ARGLESS_TEXT(CharT, "true"))
			result = true;
		else if (value_lower == _ARGLESS_TEXT(CharT, "false"))
			result = false;
		else
			throw ArgType::Boolean;

		args.consume();
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_character<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Character;
		auto& value = *arg.value();

		if (value.size() != 1)
			throw ArgType::Character;

		T result = _ARGLESS_CONVERT_CHAR(CharT, T, value[0]);

		args.consume();
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_integer<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Integer;
		auto& value = *arg.value();

		T result;
		try
		{
			result = stot<T>(value);
		}
		catch (...)
		{
			throw ArgType::Integer;
		}

		args.consume();
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_floating_point<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::FloatingPoint;
		auto& value = *arg.value();

		T result;
		try
		{
			result = stot<T>(value);
		}
		catch (...)
		{
			throw ArgType::FloatingPoint;
		}

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Boolean");
	}
	template <typename T, typename CharT, std::enable_if_t<is_character<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Character");
	}
	template <typename T, typename CharT, std::enable_if_t<is_integer<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Integer");
	}
	template <typename T, typename CharT, std::enable_if_t<is_floating_point<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Float");
	}
};

_ARGLESS_PRIVATE_END
