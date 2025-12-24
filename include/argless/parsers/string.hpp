#pragma once
#include "../parser.hpp"

#if defined(ARGLESS_HEADER_STRING) || defined(_GLIBCXX_STRING) || defined(_LIBCPP_STRING) || defined(_STRING_)

#include <string>

_ARGLESS_CORE_BEGIN

template <typename str_char, typename str_traits, typename str_alloc>
struct parser<std::basic_string<str_char, str_traits, str_alloc>>
{
	using type = std::basic_string<str_char, str_traits, str_alloc>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>;
		auto& value = *arg;

		if constexpr (std::is_same_v<str_char, char> && std::is_same_v<char_t, char>)
			return args.consume(), type(value);
		else
		{
			std::size_t size = slen(value);

			type result;
			result.reserve(size);

			for (auto it = value; it < value + size;)
				result += get_charu<str_char>(it).data();

			return args.consume(), std::move(result);
		}
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char, "text">();
};

_ARGLESS_CORE_END

#endif

#if defined(ARGLESS_HEADER_STRING_VIEW) || defined(_GLIBCXX_STRING_VIEW) || defined(_LIBCPP_STRING_VIEW) || defined(_STRING_VIEW_)

#include <string_view>

_ARGLESS_CORE_BEGIN

template <typename str_char, typename str_traits>
struct parser<std::basic_string_view<str_char, str_traits>>
{
	using type = std::basic_string_view<str_char, str_traits>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		static_assert(std::same_as<char_t, str_char>, "argless parse char_t is diffrent then string_view char_t");

		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		auto& value = *arg;

		return args.consume(), type(value, value + slen(value));
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char, "text">();
};

_ARGLESS_CORE_END
 
#endif

_ARGLESS_CORE_BEGIN

template <typename str_char>
struct parser<const str_char*>
{
	using type = const str_char*; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		static_assert(std::same_as<char_t, str_char>, "argless parse char_t is diffrent then ptr view char_t");

		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		auto& value = *arg;

		return args.consume(), value;
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char, "text">();
};

_ARGLESS_CORE_END
