#pragma once
#include "Core.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <stdexcept>

_ARGLESS_PRIVATE_BEGIN

template <typename T, typename CharT>
T stot(std::basic_string_view<CharT> str)
{
	if constexpr (std::is_same_v<T, float>)
		return static_cast<T>(std::stof(std::string(str)));
	else if constexpr (std::is_same_v<T, double>)
		return static_cast<T>(std::stod(std::string(str)));
	else if constexpr (std::is_same_v<T, long double>)
		return static_cast<T>(std::stold(std::string(str)));
	else if constexpr (std::is_integral_v<T>)
	{
		if constexpr (std::is_signed_v<T>)
			return static_cast<T>(std::stoll(std::string(str)));
		else
			return static_cast<T>(std::stoull(std::string(str)));
	}

	throw std::invalid_argument("invalid stot argument");
}

_ARGLESS_PRIVATE_END
