#pragma once
#include <string>
#include <algorithm>
#include <iterator>
#include "../Args.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class String
{
	template <typename T>
	struct is_string : std::false_type {};
	template <typename T>
	struct is_string<std::basic_string<T>> : std::true_type {};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_string<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::String;
		auto& value = *arg.value();

		T result;
		std::transform(value.begin(), value.end(), std::back_inserter(result), [](CharT c) -> typename T::value_type { return _ARGLESS_CONVERT_CHAR(CharT, typename T::value_type, c); });

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Word");
	}
};

_ARGLESS_PRIVATE_END
