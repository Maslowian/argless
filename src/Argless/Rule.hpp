#pragma once
#include "Core.hpp"
#include "Args.hpp"

#include <unordered_map>
#include <string>
#include <optional>

_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
struct EmptyRule
{
public:
	using Parser = std::function<std::optional<void*>(Args<CharT>&)>;
	using Deleter = std::function<void(void*)>;

public:
	Parser parser;
	size_t type_hash;
	Deleter deleter;
	bool is_rule_vector;
	std::optional<std::basic_string<CharT>> short_description;
	std::optional<std::basic_string<CharT>> description;
	std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)> type_description;
};

template <typename CharT>
struct Rule : EmptyRule<CharT>
{
public:
	using Parser = typename EmptyRule<CharT>::Parser;
	using Deleter = typename EmptyRule<CharT>::Deleter;

public:
	std::vector<std::basic_string<CharT>> shortcuts;
};

template <typename CharT>
struct EnumRule
{
public:
	using EnumMap = std::unordered_map<std::basic_string<CharT>, intmax_t>;

public:
	EnumMap enum_map;
};

_ARGLESS_PRIVATE_END
