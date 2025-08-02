#pragma once
#include <vector>

#include "Core.hpp"

_ARGLESS_BEGIN

template <typename T, typename AT = std::allocator<T>>
class RuleVector : public std::vector<T, AT>
{
public:
	RuleVector() = default;
	~RuleVector() = default;
};

_ARGLESS_END

_ARGLESS_PRIVATE_BEGIN

template <typename T>
struct is_rule_vector : std::false_type {};
template <typename T, typename A>
struct is_rule_vector<RuleVector<T, A>> : std::true_type {};

template <typename T>
struct type_under
{
	using type = T;
};
template <typename T, typename A>
struct type_under<RuleVector<T, A>>
{
	using type = T;
};

_ARGLESS_PRIVATE_END
