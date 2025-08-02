#pragma once
#include <array>
#include "Parser.hpp"

_ARGLESS_PRIVATE_BEGIN

class Array
{
	template <typename T>
	struct is_array : std::false_type {};
	template <typename T, size_t N>
	struct is_array<std::array<T, N>> : std::true_type {};

	template <typename T>
	struct array_info;
	template <typename T, size_t N>
	struct array_info<std::array<T, N>>
	{
		using value_type = T;
		static constexpr size_t size = N;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_array<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		T result;

		for (size_t i = 0; i < result.size(); i++)
		{
			auto value = get_parser<CharT, typename T::value_type>()(args);
			result[i] = value;
		}

		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_type_description<typename T::value_type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ">[") + std::to_string(array_info<T>::size) + _ARGLESS_CHAR(CharT, ']');
	}
};

_ARGLESS_PRIVATE_END
