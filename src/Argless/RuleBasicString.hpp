#pragma once
#include <string>

#include "Core.hpp"

_ARGLESS_BEGIN

template <typename CT, typename T = std::char_traits<CT>, typename AT = std::allocator<CT>> 
class RuleBasicString : public std::basic_string<CT, T, AT>
{
public:
	using non_rule_basic_string = std::basic_string<CT, T, AT>;

	RuleBasicString() = default;
	~RuleBasicString() = default;
};

_ARGLESS_END

_ARGLESS_PRIVATE_BEGIN

template <typename T>
struct is_rule_basic_string : std::false_type {};
template <typename CT, typename T, typename AT>
struct is_rule_basic_string<RuleBasicString<CT, T, AT>> : std::true_type {};

_ARGLESS_PRIVATE_END
