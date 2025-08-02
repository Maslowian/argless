#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <string_view>
#include "Core.hpp"

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
struct Args
{
	std::vector<std::basic_string_view<CharT>> args;
	size_t index = 0;
	std::optional<size_t> limit = std::nullopt;

	std::function<bool()> is_rule;
	std::function<std::optional<intmax_t>(size_t)> is_enum_value;

public:
	inline std::optional<std::basic_string_view<CharT>*> peak()
	{
		if (is_rule())
			return std::nullopt;
		return peak_rule();
	}

	inline void consume()
	{
		index++;
	}

public:
	inline std::optional<std::basic_string_view<CharT>*> peak_rule()
	{
		if (index < args.size() && (!limit.has_value() || index < limit.value()))
			return &args[index];
		return std::nullopt;
	}
};

_ARGLESS_PRIVATE_END
