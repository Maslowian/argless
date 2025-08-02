#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "RuleVector.hpp"
#include "Core.hpp"
#include "Rule.hpp"
#include "ArglessData.hpp"
#include "Parser.hpp"
#include "Result.hpp"
#include "Exception.hpp"
#include "Helper.hpp"

_ARGLESS_BEGIN

template <typename CharT>
class BasicArgless
{
	using string_view = std::basic_string_view<CharT>;
	using string = std::basic_string<CharT>;

	struct void_t {};

public:
	template <typename OutT = _ARGLESS_PRIVATE OutStream<CharT>>
	constexpr BasicArgless()
		: m_helper(new _ARGLESS_PRIVATE DefaultHelper<CharT, OutT>(std::nullopt))
	{}

	template <typename OutT = _ARGLESS_PRIVATE OutStream<CharT>>
	constexpr BasicArgless(const string& description)
		: m_helper(new _ARGLESS_PRIVATE DefaultHelper<CharT, OutT>(description))
	{}

	constexpr BasicArgless(std::unique_ptr<_ARGLESS_PRIVATE Helper<CharT>> helper)
		: m_helper(std::move(helper))
	{}

public:
	template <typename T>
	constexpr void set_enum_map(const std::unordered_map<string, T>& map)
	{
		_ARGLESS_STATIC_ASSERT(std::is_enum_v<T>, "T must be an enum type");

		typename _ARGLESS_PRIVATE EnumRule<CharT>::EnumMap enum_map;
		for (const auto& [key, value] : map)
			enum_map[key] = static_cast<intmax_t>(value);

		m_data.enum_rules.emplace(typeid(T).hash_code(), _ARGLESS_PRIVATE EnumRule<CharT>{ .enum_map = enum_map });
	}
	
	template <typename T>
	constexpr void set_rule(const string& name, const std::vector<string>& shortcuts, const std::optional<string>& short_description = {}, const std::optional<string>& description = {})
	{
		_ARGLESS_PRIVATE Rule<CharT> rule;

		rule.parser = typename _ARGLESS_PRIVATE Rule<CharT>::Parser(&parser_caller<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.type_hash = typeid(T).hash_code();
		rule.deleter = &deleter<typename _ARGLESS_PRIVATE type_under<T>::type>;
		rule.is_rule_vector = _ARGLESS_PRIVATE is_rule_vector<T>::value;
		rule.type_description = std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)>(&type_description<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.short_description = short_description;
		rule.description = description;

		rule.shortcuts = shortcuts;

		m_data.rules.emplace(name, rule);
	}

	template <typename T>
	constexpr void set_rule(std::nullptr_t, const std::optional<string>& short_description = {}, const std::optional<string>& description = {})
	{
		_ARGLESS_PRIVATE EmptyRule<CharT> rule;

		rule.parser = typename _ARGLESS_PRIVATE Rule<CharT>::Parser(&parser_caller<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.type_hash = typeid(T).hash_code();
		rule.deleter = &deleter<typename _ARGLESS_PRIVATE type_under<T>::type>;
		rule.is_rule_vector = _ARGLESS_PRIVATE is_rule_vector<T>::value;
		rule.type_description = std::function<std::basic_string<CharT>(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)>(&type_description<typename _ARGLESS_PRIVATE type_under<T>::type>);
		rule.short_description = short_description;
		rule.description = description;

		m_data.empty_rule = rule;
	}

public:
	template <typename T>
	constexpr void remove_enum_map()
	{
		_ARGLESS_STATIC_ASSERT(std::is_enum_v<T>, "T must be an enum type");

		m_data.enum_rules.erase(typeid(T).hash_code());
	}

	constexpr void remove_rule(const string& name)
	{
		m_data.rules.erase(name);
	}

	constexpr void remove_rule(std::nullptr_t)
	{
		m_data.empty_rule = std::nullopt;
	}

public:
	constexpr void set_helper(std::unique_ptr<_ARGLESS_PRIVATE Helper<CharT>> helper)
	{
		m_helper = std::move(helper);
	}

	void help()
	{
		_ARGLESS_ASSERT(m_helper, "Helper is not set");
		m_helper->info(m_data);
	}

	void help(const string& name)
	{
		_ARGLESS_ASSERT(m_helper, "Helper is not set");
		m_helper->info(name, m_data);
	}

public:
	std::optional<Result> parse(int argc, const CharT* const* argv) const 
	{
		_ARGLESS_PRIVATE Args<CharT> args;
		std::vector<string_view> args_vector;
		args_vector.reserve(argc);
		for (int i = 0; i < argc; i++)
			args_vector.push_back(argv[i]);
		args.args = args_vector;
		args.is_rule = std::bind(&BasicArgless::is_rule, this, &args);
		args.is_enum_value = std::bind(&BasicArgless::is_enum_value, this, &args, std::placeholders::_1);

		Result result;

		if (args.index < argc)
		{
			result.executable_path = args.args[args.index];
			args.consume();
		}

		try
		{
			while (args.index < argc)
			{
				const _ARGLESS_PRIVATE EmptyRule<CharT>* rule;
				std::variant<std::monostate, string, _ARGLESS_PRIVATE NoRule> rule_name;
				std::function<void(std::shared_ptr<void>)> add_value;
				std::function<size_t()> rule_value_size;

				auto named_rule = find_rule(string(args.args[args.index]));
				if (named_rule != m_data.rules.end())
				{
					args.consume();

					rule = &named_rule->second;
					rule_name = named_rule->first;

					add_value = [&result, &named_rule](std::shared_ptr<void> value)
					{
						auto result_value = result.values.find(named_rule->first);
						if (result_value == result.values.end())
						{
							result.values.insert(std::make_pair(named_rule->first, _ARGLESS_PRIVATE ResultValue(named_rule->second.type_hash)));
							result_value = result.values.find(named_rule->first);
						}
						result_value->second.values.push_back(value);
					};

					rule_value_size = [&result, &named_rule]()
					{
						auto result_value = result.values.find(named_rule->first);
						if (result_value == result.values.end())
							return size_t(0);
						return result_value->second.values.size();
					};
				}
				else
				{
					if (!m_data.empty_rule.has_value())
						throw _ARGLESS_PRIVATE ArglessException<CharT>(_ARGLESS_PRIVATE ArglessExceptionType::StrayArgument, std::monostate(), args, m_data);

					rule = &m_data.empty_rule.value();
					rule_name = _ARGLESS_PRIVATE NoRule();
					add_value = [this, &result](std::shared_ptr<void> value)
					{
						if (!result.empty_rule_value.has_value())
							result.empty_rule_value = _ARGLESS_PRIVATE ResultValue(this->m_data.empty_rule.value().type_hash);
						result.empty_rule_value->values.push_back(value);
					};

					rule_value_size = [&result]()
					{
						if (!result.empty_rule_value.has_value())
							return size_t(0);
						return result.empty_rule_value->values.size();
					};
				}

				if (!rule->is_rule_vector && rule_value_size() > 0)
					throw _ARGLESS_PRIVATE ArglessException<CharT>(_ARGLESS_PRIVATE ArglessExceptionType::ValueDuplication, rule_name, args, m_data);

				try 
				{
					auto value = rule->parser(args);
					add_value(std::shared_ptr<void>(value.value(), rule->deleter));
				} 
				catch (_ARGLESS_PRIVATE ArgType e)
				{
					throw _ARGLESS_PRIVATE ArglessException<CharT>(_ARGLESS_PRIVATE ArglessExceptionType::WrongArgument, rule_name, args, m_data, e);
				}
			}
		}
		catch (const _ARGLESS_PRIVATE ArglessException<CharT>& e)
		{
			if (m_helper)
			{
				m_helper->help(e);
				return std::nullopt;
			}
			else
				throw e;
		}

		return result;
	}

private:
	template <typename T>
	static void* parser_caller(_ARGLESS_PRIVATE Args<CharT>& args)
	{
		auto result = _ARGLESS_PRIVATE get_parser<CharT, T>()(args);
		return static_cast<void*>(new T(result));
	}

	template <>
	static void* parser_caller<void>(_ARGLESS_PRIVATE Args<CharT>& args)
	{
		return static_cast<void*>(new void_t());
	}

private:
	template <typename T>
	static void deleter(void* ptr)
	{
		delete static_cast<T*>(ptr);
	}

	template <>
	static void deleter<void>(void* ptr)
	{
		delete static_cast<void_t*>(ptr);
	}

private:
	template <typename T>
	static string type_description(const std::function<std::vector<string>(size_t)>& enum_info)
	{
		return _ARGLESS_PRIVATE get_type_description<T, CharT>(enum_info);
	}

	template <>
	static string type_description<void>(const std::function<std::vector<string>(size_t)>& enum_info)
	{
		return _ARGLESS_TEXT(CharT, "Flag");
	}

private:
	bool is_rule(_ARGLESS_PRIVATE Args<CharT>* args) const
	{
		if (args->index >= args->args.size())
			return false;
		return find_rule(string(args->args[args->index])) != m_data.rules.end();
	}

	std::optional<intmax_t> is_enum_value(_ARGLESS_PRIVATE Args<CharT>* args, size_t enum_hash) const
	{
		auto enum_rule = m_data.enum_rules.find(enum_hash);
		_ARGLESS_ASSERT(enum_rule != m_data.enum_rules.end(), "Enum is not set");

		auto value = enum_rule->second.enum_map.find(string(args->args[args->index]));
		if (value == enum_rule->second.enum_map.end())
			return std::nullopt;

		return value->second;
	}

private:
	typename std::unordered_map<string, _ARGLESS_PRIVATE Rule<CharT>>::const_iterator find_rule(const string& arg) const
	{
		{
			auto rule = m_data.rules.find(arg);
			if (rule != m_data.rules.end())
				return rule;
		}

		for (auto rule = m_data.rules.begin(); rule != m_data.rules.end(); rule++)
		{
			if (std::find(rule->second.shortcuts.begin(), rule->second.shortcuts.end(), arg) != rule->second.shortcuts.end())
				return rule;
		}

		return m_data.rules.end();
	}

private:
	_ARGLESS_PRIVATE ArglessData<CharT> m_data;
	std::unique_ptr<_ARGLESS_PRIVATE Helper<CharT>> m_helper;
};

typedef BasicArgless<char> Argless;
typedef BasicArgless<wchar_t> WArgless;

_ARGLESS_END
