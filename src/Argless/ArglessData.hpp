#pragma once
#include <unordered_map>
#include <optional>
#include <string>

#include "Core.hpp"
#include "Rule.hpp"

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
struct ArglessData
{
	std::unordered_map<std::basic_string<CharT>, _ARGLESS_PRIVATE Rule<CharT>> rules; 
	std::unordered_map<size_t, _ARGLESS_PRIVATE EnumRule<CharT>> enum_rules;
	std::optional<_ARGLESS_PRIVATE EmptyRule<CharT>> empty_rule;
};

_ARGLESS_PRIVATE_END
