#pragma once
#include "../parser.hpp"

#if defined(ARGLESS_STDH_ARRAY) || defined(_GLIBCXX_ARRAY) || defined(_LIBCPP_ARRAY) || defined(_ARRAY_)

#include <array>

_ARGLESS_CORE_BEGIN

template <typename arr_t>
struct parser<std::array<arr_t, 0>>
{
	using type = std::array<arr_t, 0>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		return make_default(type());
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t>("<>");
};

template <typename arr_t, std::size_t arr_n>
struct parser<std::array<arr_t, arr_n>>
{
	using type = std::array<arr_t, arr_n>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		type value;

		bool all_default = true;
		for (size_t i = 0; i < value.size(); ++i)
		{
			auto result = parser<arr_t>::parse(args);
			if (!result.is_valid())
				return std::move(result).expected();

			if (!result.is_default())
				all_default = false;

			value[i] = std::move(result).get();
		}

		if (all_default)
			return make_default(std::move(value));
		else
			return value;
	}

	template <typename char_t>
	static constexpr inline auto name = array_wrap_name<type_name<arr_t, char_t>()>() + str_from<char_t, "[">() + number_name<char_t, arr_n>() + str_from<char_t, "]">();
};

_ARGLESS_CORE_END

#endif
