#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Core.hpp"
#include "RuleVector.hpp"

_ARGLESS_PRIVATE_BEGIN

struct ResultValue
{
	ResultValue (size_t type_hash) : type_hash(type_hash)
	{}

	size_t type_hash;
	std::vector<std::shared_ptr<void>> values;
};

_ARGLESS_PRIVATE_END

_ARGLESS_BEGIN

template <typename CharT>
class BasicArgless;

template <typename CharT>
class BasicResult
{
	using string = std::basic_string<CharT>;

private:
	BasicResult() = default;

public:
	template <typename T>
	std::shared_ptr<T> get(const string& name) const
	{
		auto value = values.find(name);
		
		if (value == values.end())
			return nullptr;

		return get<T>(value->second);
	}

	template <typename T>
	std::shared_ptr<T> get(std::nullptr_t) const
	{
		if (!empty_rule_value.has_value())
			return nullptr;

		return get<T>(empty_rule_value.value());
	}

public:
	size_t get(const string& name) const
	{
		auto value = values.find(name);
		if (value == values.end())
			return 0;
		
		return value->second.values.size();
	}

	size_t get(std::nullptr_t) const
	{
		if (!empty_rule_value.has_value())
			return 0;

		return empty_rule_value.value().values.size();
	}

public:
	string path() const
	{
		return executable_path;
	}

private:
	template <typename T>
	std::shared_ptr<T> get(const _ARGLESS_PRIVATE ResultValue& result_value) const
	{
		_ARGLESS_ASSERT(result_value.type_hash == typeid(T).hash_code(), "Type mismatch");

		if constexpr (_ARGLESS_PRIVATE is_rule_vector<T>::value)
		{
			T result;
			for (size_t i = 0; i < result_value.values.size(); i++)
				result.push_back(*reinterpret_cast<typename _ARGLESS_PRIVATE type_under<T>::type*>(result_value.values[i].get()));

			auto test = std::shared_ptr<T>(new T(result));
			return test;
		}
		else
		{
			if (result_value.values.empty())
				return nullptr;
			return std::static_pointer_cast<T>(result_value.values[0]);
		}
	}

private:
	std::unordered_map<string, _ARGLESS_PRIVATE ResultValue> values;
	std::optional<_ARGLESS_PRIVATE ResultValue> empty_rule_value;
	string executable_path;

private:
	friend class BasicArgless<CharT>;
};


using Result = BasicResult<char>;
using WResult = BasicResult<wchar_t>;

_ARGLESS_END
