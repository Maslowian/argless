#pragma once
#include <tuple>
#include "Parser.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class Tuple
{
	template <typename T>
	struct is_filled_tuple : std::false_type {};
	template <typename T, typename ...Rest>
	struct is_filled_tuple<std::tuple<T, Rest...>> : std::true_type {};
	
	template <typename T>
	struct is_empty_tuple : std::false_type {};
	template <>
	struct is_empty_tuple<std::tuple<>> : std::true_type {};

	template <typename T>
	struct is_single_tuple : std::false_type {};
	template <typename T>
	struct is_single_tuple<std::tuple<T>> : std::true_type {};

	template <typename T>
	struct is_tail_tuple
	{
		static constexpr inline bool value = is_filled_tuple<T>::value && !is_single_tuple<T>::value;
	};

	template <typename T>
	struct is_tuple : std::false_type {};
	template <typename ...Args>
	struct is_tuple<std::tuple<Args...>> : std::true_type {};

	template <typename T>
	struct tuple_info
	{
		_ARGLESS_STATIC_ASSERT(!is_empty_tuple<T>::value, "Cannot get tuple info from empty tuple");
	};
	template <typename T, typename ...Args>
	struct tuple_info<std::tuple<T, Args...>>
	{
		using type = T;
		using tail = std::tuple<Args...>;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_tuple<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_tail_tuple<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto start_index = args.index;
		auto orginal_limit = args.limit;
		std::optional<size_t> limit = orginal_limit;
		typename Tuple::tuple_info<T>::type value;
		std::optional<typename Tuple::tuple_info<T>::tail> rest;

		std::optional<std::pair<ArgType, size_t>> e_before;

		do
		{
			args.index = start_index;
			args.limit = limit;

			try
			{
				value = get_parser<CharT, typename Tuple::tuple_info<T>::type>()(args);
			}
			catch (ArgType e)
			{
				if (e_before.has_value())
				{
					args.index = e_before.value().second;
					throw e_before.value().first;
				}
				else
					throw e;
			}

			if (!limit.has_value())
				limit = args.index;

			args.limit = orginal_limit;
			try
			{
				rest = get_parser<CharT, typename Tuple::tuple_info<T>::tail>()(args);
			}
			catch (ArgType e)
			{
				if (limit.value() <= start_index)
					throw e;
				limit.value()--;

				e_before = std::make_pair(e, args.index);
			}
			if (rest.has_value())
				break;
		} 
		while (true);

		args.limit = orginal_limit;

		T result = std::tuple_cat(std::make_tuple(value), rest.value());
		return result;
	}
	template <typename CharT, typename T, std::enable_if_t<is_single_tuple<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto value = get_parser<CharT, typename Tuple::tuple_info<T>::type>()(args);
		T result = std::make_tuple(value);
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_simple_type_description<T, CharT>(enum_info) + _ARGLESS_CHAR(CharT, '>');
	}

private:
	template <typename T, typename CharT, std::enable_if_t<is_tail_tuple<T>::value, int> = 0>
	static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return get_type_description<typename Tuple::tuple_info<T>::type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ", ") + get_simple_type_description<typename Tuple::tuple_info<T>::tail, CharT>(enum_info);
	}

	template <typename T, typename CharT, std::enable_if_t<is_single_tuple<T>::value, int> = 0>
	static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return get_type_description<typename Tuple::tuple_info<T>::type, CharT>(enum_info);
	}

	template <typename T, typename CharT, std::enable_if_t<!is_t<T>::value, int> = 0>
	static std::string get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return get_type_description<T, CharT>(enum_info);
	}
};

_ARGLESS_PRIVATE_END
