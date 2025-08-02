#pragma once
#include "Parser.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class Optional
{
	template <typename T>
	struct is_optional : std::false_type {};
	template <typename T>
	struct is_optional<std::optional<T>> : std::true_type {};

	friend class DynamicArray;

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_optional<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		try
		{
			return get_parser<CharT, typename T::value_type>()(args);
		}
		catch (ArgType)
		{
			return std::nullopt;
		}
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '<') + get_type_description<typename T::value_type, CharT>(enum_info) + _ARGLESS_TEXT(CharT, ">?");
	}
};

_ARGLESS_PRIVATE_END
