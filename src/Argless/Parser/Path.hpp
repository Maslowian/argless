#pragma once
#include <filesystem>
#include <string>
#include <algorithm>
#include <iterator>
#include "../Args.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class Path
{
	template <typename T>
	struct is_path : std::false_type {};
	template <>
	struct is_path<std::filesystem::path> : std::true_type {};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_path<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Path;
		auto& value = *arg.value();

		typename T::string_type result;
		std::transform(value.begin(), value.end(), std::back_inserter(result), [](CharT c) -> typename T::value_type { return _ARGLESS_CONVERT_CHAR(CharT, typename T::value_type, c); });

		args.consume();
		return T(result);
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "Path");
	}
};

_ARGLESS_PRIVATE_END
