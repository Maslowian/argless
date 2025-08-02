#pragma once
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>
#include "Parser.hpp"
#include "Optional.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class DynamicArray
{
	template <typename T>
	struct is_vector : std::false_type {};
	template <typename T>
	struct is_vector<std::vector<T>> : std::true_type {};

	template <typename T>
	struct is_deque : std::false_type {};
	template <typename T>
	struct is_deque<std::deque<T>> : std::true_type {};

	template <typename T>
	struct is_list : std::false_type {};
	template <typename T>
	struct is_list<std::list<T>> : std::true_type {};

	template <typename T>
	struct is_set : std::false_type {};
	template <typename T>
	struct is_set<std::set<T>> : std::true_type {};

	template <typename T>
	struct is_unordered_set : std::false_type {};
	template <typename T>
	struct is_unordered_set<std::unordered_set<T>> : std::true_type {};


public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_vector<T>::value || is_deque<T>::value || is_list<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		T result;

		while (true)
		{
			try
			{
				auto value = get_parser<CharT, typename T::value_type>()(args);

				if constexpr (Optional::is_optional<typename T::value_type>::value)
					if (!value.has_value())
						break;

				result.push_back(value);
			} 
			catch (ArgType) 
			{
				break;
			}
		}

		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_type_description<typename T::value_type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ">[...]");
	}
};

_ARGLESS_PRIVATE_END
