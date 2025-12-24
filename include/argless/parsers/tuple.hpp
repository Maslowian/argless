#pragma once
#include "../parser.hpp"

#if defined(ARGLESS_STDH_TUPLE) || defined(_GLIBCXX_TUPLE) || defined(_LIBCPP_TUPLE) || defined(_TUPLE_)

#include <tuple>

_ARGLESS_CORE_BEGIN

template <typename tup_t, typename tup_st, typename... tup_ts>
struct parser<std::tuple<tup_t, tup_st, tup_ts...>>
{
	using type = std::tuple<tup_t, tup_st, tup_ts...>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		const auto from = args.m_index; 
		const auto to = args.m_limit;

		for (size_t l = args.m_limit; l >= from; --l)
		{
			args.m_index = from;
			args.m_limit = l;

			auto result = parser<tup_t>::parse(args);

			if (!result.is_valid())
				continue;

			args.m_limit = to;
			auto rest_result = parser<std::tuple<tup_st, tup_ts...>>::parse(args);

			if (!result.is_valid())
				continue;

			if (result.is_default() && rest_result.is_default())
				return make_default(std::tuple_cat(std::make_tuple(std::move(result.get())), std::move(rest_result.get())));
			else
				return std::tuple_cat(std::make_tuple(std::move(result.get())), std::move(rest_result.get()));
		}

		args.m_index = from;
		args.m_limit = to;
		return make_expected<type>;
	}

	template <typename char_t>
	static constexpr inline auto name = and_name<str_name<tup_t, char_t>(), str_name<std::tuple<tup_st, tup_ts...>, char_t>()>();
};

template <typename tup_t>
struct parser<std::tuple<tup_t>>
{
	using type = std::tuple<tup_t>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto result = parser<tup_t>::parse(args);

		if (!result.is_valid())
			return make_expected<type>;

		if (result.is_default())
			return make_default(type(std::move(result).get()));
		else
			return type(std::move(result).get());
	}

	template <typename char_t>
	static constexpr inline auto name = str_name<tup_t, char_t>();
};

template <>
struct parser<std::tuple<>>
{
	using type = std::tuple<>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		return make_default(type());
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t>("<>");
};

_ARGLESS_CORE_END

#endif
