#pragma once
#include <iostream>
#include <variant>
#include <filesystem>

#include "Parser/Parser.hpp"
#include "Core.hpp"
#include "Exception.hpp"


_ARGLESS_PRIVATE_BEGIN

template <typename CharT>
class Helper
{
public:
	Helper() = default;
	virtual ~Helper() = default;

public:
	virtual void help(const ArglessException<CharT>& e) = 0;
	virtual void info(const ArglessData<CharT>& data) = 0;
	virtual void info(const std::basic_string<CharT>& name, const ArglessData<CharT>& data) = 0;
};

template <typename T>
struct OutStream
{
	static inline std::ostream& out = std::cout;
};
template <>
struct OutStream<wchar_t>
{
	static inline std::wostream& out = std::wcout;
};

template <typename CharT, typename OutT = OutStream<CharT>>
class DefaultHelper : public Helper<CharT>
{
	static inline auto& out = OutT::out;

public:
	DefaultHelper(const std::optional<std::basic_string<CharT>>& description) : Helper<CharT>(), m_description(description)
	{}

public:
	void help(const ArglessException<CharT>& e) override
	{
		error_description(e);
		hint(e);
		where(e);
	}

	void info(const ArglessData<CharT>& data) override
	{
		if (m_description.has_value())
			out << m_description.value() << std::endl << std::endl;

		out << _ARGLESS_TEXT(CharT, "Options:") << std::endl;

		for (const auto& [name, rule] : data.rules)
		{
			out << _ARGLESS_TEXT(CharT, "\t");

			out << name;

			if (rule.short_description.has_value())
			{
				out << _ARGLESS_TEXT(CharT, " - ");
				out << rule.short_description.value();
			}

			out << std::endl;
		}
	}

	void info(const std::basic_string<CharT>& name, const ArglessData<CharT>& data) override
	{
		auto rule = data.rules.find(name);
		if (rule == data.rules.end())
			for (const auto& [name_, rule_] : data.rules)
				if (std::find(rule_.shortcuts.begin(), rule_.shortcuts.end(), name) != rule_.shortcuts.end())
				{
					rule = data.rules.find(name_);
					break;
				}
		if (rule == data.rules.end())
		{
			out << _ARGLESS_TEXT(CharT, "Option '") << name << _ARGLESS_TEXT(CharT, "' not found") << std::endl;
			return;
		}

		out << _ARGLESS_TEXT(CharT, "Option: ") << rule->first << std::endl;

		auto enum_info = [&data](size_t enum_hash)-> std::vector<std::basic_string<CharT>> 
		{ 
			auto it = data.enum_rules.find(enum_hash);

			_ARGLESS_ASSERT(it != data.enum_rules.end(), "Enum not found");

			auto& enum_map = it->second.enum_map;

			std::vector<std::basic_string<CharT>> result;

			for (const auto& [name, value] : enum_map)
				result.push_back(name);

			return result;
		};

		out << _ARGLESS_TEXT(CharT, "Value type: ") << rule->second.type_description(enum_info) << std::endl;

		if (rule->second.description.has_value())
			out << _ARGLESS_TEXT(CharT, "Description: ") << rule->second.description.value() << std::endl;
		else if (rule->second.short_description.has_value())
			out << _ARGLESS_TEXT(CharT, "Description: ") << rule->second.short_description.value() << std::endl;

		if (rule->second.shortcuts.size() > 0)
		{
			out << _ARGLESS_TEXT(CharT, "Shortcuts: ");

			for (const auto& shortcut : rule->second.shortcuts)
			{
				if (shortcut == rule->first)
					continue;

				if (shortcut != rule->second.shortcuts.front())
					out << _ARGLESS_TEXT(CharT, ", ");

				out << shortcut;
			}
		}

		out << std::endl;
	}

protected:
	std::optional<std::basic_string<CharT>> m_description;

protected:
	void error_description(const ArglessException<CharT>& e) const
	{
		if (e.type == ArglessExceptionType::WrongArgument)
		{
			if (e.arg_type.has_value() && e.arg_type.value() == ArgType::Variant)
			{
				out << _ARGLESS_TEXT(CharT, "Wrong argument: ");

				out << _ARGLESS_TEXT(CharT, "Given arguments do not match any of the expected types");
			}
			else
			{
				if (e.args.index >= e.args.args.size())
					out << _ARGLESS_TEXT(CharT, "Missing argument: ");
				else
					out << _ARGLESS_TEXT(CharT, "Wrong argument: ");

				out << _ARGLESS_TEXT(CharT, "Expected argument of type '");

				if (e.arg_type.has_value())
					out << get_arg_type_name(e.arg_type.value());
				else
					out << _ARGLESS_TEXT(CharT, "Unknown type");

				out << _ARGLESS_TEXT(CharT, "'");
			}
		}
		else if (e.type == ArglessExceptionType::StrayArgument)
		{
			out << _ARGLESS_TEXT(CharT, "Unexpected argument: Argument does not belong to any option");
		}
		else if	(e.type == ArglessExceptionType::ValueDuplication)
		{
			out << _ARGLESS_TEXT(CharT, "Value duplication: ");

			if (auto value = std::get_if<std::basic_string<CharT>>(&e.rule_name))
				out << _ARGLESS_TEXT(CharT, "Option '") << *value << '\'';
			else if (std::holds_alternative<NoRule>(e.rule_name))
				out << _ARGLESS_TEXT(CharT, "No option specified");

			out << _ARGLESS_TEXT(CharT, " does not allow being specified more than once");
		}

		out	<< std::endl;
	}

	std::basic_string<CharT> get_arg_type_name(ArgType arg_type) const
	{
		std::function<std::vector<std::basic_string<CharT>>(size_t)> false_func;
		switch (arg_type)
		{
		case ArgType::Boolean:
			return get_type_description<bool, CharT>(false_func);
		case ArgType::Integer:
			return get_type_description<int, CharT>(false_func);
		case ArgType::FloatingPoint:
			return get_type_description<float, CharT>(false_func);
		case ArgType::String:
			return get_type_description<std::basic_string<CharT>, CharT>(false_func);
		case ArgType::Character:
			return get_type_description<CharT, CharT>(false_func);
		case ArgType::Enum:
			return "Option from the list";
		case ArgType::Variant:
			return get_type_description<bool, CharT>(false_func);
		case ArgType::Path:
			return get_type_description<std::filesystem::path, CharT>(false_func);
		}
	}

	void hint(const ArglessException<CharT>& e) const
	{
		if (!(e.type == ArglessExceptionType::StrayArgument))
			return;

		out << _ARGLESS_TEXT(CharT, "Hint: ");

		if (e.type == ArglessExceptionType::StrayArgument)
			out << _ARGLESS_TEXT(CharT, "Check expected arguments for the specified option");

		out << std::endl;
	}

	void where(const ArglessException<CharT>& e) const
	{
		size_t target_index = e.args.index;

		if (target_index > 0 && e.type == ArglessExceptionType::ValueDuplication)
			target_index--;

		size_t space = 0;

		for (size_t i = 0; i < e.args.args.size(); i++)
		{
			if (i != 0)
			{
				out << _ARGLESS_TEXT(CharT, " ");

				if (i <= target_index)
					space++;
			}

			out << e.args.args[i];

			if (i < target_index)
				space += e.args.args[i].size();
		}
		out << std::endl;

		bool is_arg = target_index < e.args.args.size() && !(e.type == ArglessExceptionType::ValueDuplication && std::holds_alternative<NoRule>(e.rule_name));
		size_t arg_size = 0;
		if (is_arg)
		{
			arg_size = e.args.args[target_index].size();
		}

		if (!is_arg)
			space++;

		for (size_t i = 0; i < space; i++)
			out << _ARGLESS_TEXT(CharT, " ");

		if (!is_arg)
			out << _ARGLESS_TEXT(CharT, "^");
		else
			for (size_t i = 0; i < arg_size; i++)
				out << _ARGLESS_TEXT(CharT, "~");

		out << std::endl;
	}
};

_ARGLESS_PRIVATE_END
