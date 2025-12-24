#pragma once
#include "../parser.hpp"

_ARGLESS_CORE_BEGIN

template <>
struct parser<bool>
{
	using type = bool; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		auto& value = *arg;

		if (seq_nocase(value, "true"))
			return args.consume(), true;
		else if (seq_nocase(value, "false"))
			return args.consume(), false;

		return make_expected<type>; 
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t, "boolean">();
};

template <typename t>
struct parser<t, std::enable_if_t<tetter<int, unsigned int, short, unsigned short, signed char, unsigned char, long, unsigned long, long long, unsigned long long>::template find_t<t>::value>>
{
	using type = t; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>;
		auto& value = *arg;

		auto integer = stot<type>(value);

		if (!integer)
			return make_expected<type>;

		return args.consume(), *integer;
	}

	template <typename char_t>
	static constexpr inline auto name = [](){
		if constexpr (std::is_unsigned_v<t>)
			return str_from<char_t, "+integer">();
		else
			return str_from<char_t, "integer">();
	}();
};

template <typename t>
struct parser<t, std::enable_if_t<tetter<float, double, long double>::template find_t<t>::value>>
{
	using type = t; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>;
		auto& value = *arg;

		auto number = stot<type>(value);

		if (!number)
			return make_expected<type>;

		return args.consume(), *number;
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t, "number">();
};

template <typename t>
struct parser<t, std::enable_if_t<tetter<char, wchar_t, char8_t, char16_t, char32_t>::template find_t<t>::value>>
{
	using type = t; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		const auto* value = *arg;

		if constexpr (std::is_same_v<t, char> && std::is_same_v<char_t, char>)
		{
			if (slen(value) != 1)
				return make_expected<char>;
			else
				return args.consume(), *value;
		}
		else
		{
			auto chars = get_charu<type>(value);

			// lenght check
			if (chars.m_buffer[0] == 0 || *value != 0)
				return make_expected<type>;

			// too big to cast
			if (chars.m_buffer[1] != 0)
			{
				if constexpr (sizeof(type) >= 2)
					return args.consume(), char_cast<type>(0xFFFD); 
				else
					return args.consume(), char_cast<type>(0x0);
			}

			return args.consume(), chars.m_buffer[0];
		}
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t, "char">();
};

template <typename t>
struct parser<t, std::enable_if_t<tetter<char[4], wchar_t[4 / sizeof(wchar_t)], char8_t[4], char16_t[2], char32_t[1]>::template find_t<t>::value>>
{
	using type = t; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		const auto* value = *arg;

		auto chars = get_charu<type>(value);

		// lenght check
		if (chars.m_buffer[0] == 0 || *value != 0)
			return make_expected<type>;

		type result;
		for (size_t i = 0; i < 4; ++i)
			static_cast<unsigned char*>(result)[i] = static_cast<unsigned char*>(chars)[i];
		return args.consume(), result;
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t, "char">();
};

_ARGLESS_CORE_END
