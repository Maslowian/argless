#pragma once
#include "Core.hpp"
#include "ArglessData.hpp"
#include "ArgType.hpp"

#include <string>
#include <exception>
#include <variant>

_ARGLESS_PRIVATE_BEGIN

enum class ArglessExceptionType
{
	WrongArgument,
	StrayArgument,
	ValueDuplication,
};

struct NoRule {};

template <typename CharT>
struct ArglessException : public std::exception
{
	ArglessException(ArglessExceptionType type, std::variant<std::monostate, std::basic_string<CharT>, NoRule> rule_name, Args<CharT> args, const ArglessData<CharT>& data, std::optional<ArgType> arg_type = std::nullopt)
		: type(type), rule_name(std::move(rule_name)), args(std::move(args)), data(data), arg_type(arg_type)
	{}

	const char* what() const noexcept override
	{
		switch (type)
		{
		case ArglessExceptionType::WrongArgument:
			return "Wrong argument";
		case ArglessExceptionType::StrayArgument:
			return "Stray argument";
		case ArglessExceptionType::ValueDuplication:
			return "Value duplication";
		}

		return "Unknown exception";
	}
	
	ArglessExceptionType type;
	std::variant<std::monostate, std::basic_string<CharT>, NoRule> rule_name;
	Args<CharT> args;
	const ArglessData<CharT>& data;
	std::optional<ArgType> arg_type;
};

_ARGLESS_PRIVATE_END
