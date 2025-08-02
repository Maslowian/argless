#pragma once
#include "../Args.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class Enum
{
public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = std::is_enum_v<T>;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_t<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		auto arg = args.peak();
		if (!arg.has_value())
			throw ArgType::Enum;

		auto value = args.is_enum_value(typeid(T).hash_code());
		if (!value.has_value())
			throw ArgType::Enum;

		T result = static_cast<T>(value.value());

		args.consume();
		return result;
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value, int> = 0> 
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		auto values = enum_info(typeid(T).hash_code());

		std::basic_string<CharT> result;
		result += _ARGLESS_CHAR(CharT, '[');
		for (size_t i = 0; i < values.size(); i++)
		{
			if (i != 0)
				result += _ARGLESS_CHAR(CharT, '|');

			result += _ARGLESS_CHAR(CharT, '\'');
			result += values[i];
			result += _ARGLESS_CHAR(CharT, '\'');
		}
		result += _ARGLESS_CHAR(CharT, ']');

		return result;
	}
};

_ARGLESS_PRIVATE_END
