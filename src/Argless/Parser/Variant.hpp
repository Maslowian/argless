#pragma once
#include <variant>
#include "Parser.hpp"
#include "../ArgType.hpp"

_ARGLESS_PRIVATE_BEGIN

class Variant
{
	template <typename T>
	struct is_variant : std::false_type {};
	template <typename ...Args>
	struct is_variant<std::variant<Args...>> : std::true_type {};

	template <typename T>
	struct is_filled_variant : std::false_type {};
	template <typename T, typename ...Args>
	struct is_filled_variant<std::variant<T, Args...>> : std::true_type {};

	template <typename T>
	struct is_empty_variant : std::false_type {};
	template <>
	struct is_empty_variant<std::variant<>> : std::true_type {};

	template <typename T>
	struct is_monostate : std::false_type {};
	template <>
	struct is_monostate<std::monostate> : std::true_type {};

	template <typename T>
	struct variant_info
	{
		_ARGLESS_STATIC_ASSERT(!is_empty_variant<T>::value, "Cannot get variant info from empty variant");
	};
	template <typename T, typename ...Args>
	struct variant_info<std::variant<T, Args...>>
	{
		using type = T;
		using tail = std::variant<Args...>;
	};

public:
	template <typename T>
	struct is_t
	{
		static inline constexpr bool value = is_variant<T>::value || is_monostate<T>::value;
	};

public:
	template <typename CharT, typename T, std::enable_if_t<is_variant<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		return VariantParser<CharT, T, std::variant_size_v<T>>::get(args);
	}

	template <typename CharT, typename T, std::enable_if_t<is_monostate<T>::value, int> = 0>
	static T get(Args<CharT>& args)
	{
		return T{};
	}

public:
	template <typename T, typename CharT, std::enable_if_t<is_t<T>::value && !is_monostate<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
	{
		return _ARGLESS_CHAR(CharT, '[') + VariantDescription<CharT, T, std::variant_size_v<T>>::get_simple_type_description(enum_info) + _ARGLESS_CHAR(CharT, ']');
	}

	template <typename T, typename CharT, std::enable_if_t<is_monostate<T>::value, int> = 0>
	static std::basic_string<CharT> get_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>&)
	{
		return _ARGLESS_TEXT(CharT, "None");
	}

private:
	template <typename CharT, typename T, size_t I>
	struct VariantParser
	{
		static T get(Args<CharT>& args)
		{
			auto index = args.index;
			try 
			{
				return get_parser<CharT, std::variant_alternative_t<I - 1, T>>()(args);
			}
			catch (ArgType e)
			{
				args.index = index;
				return VariantParser<CharT, T, I - 1>::get(args);
			}
		}

	};

	template <typename CharT, typename T>
	struct VariantParser<CharT, T, 0>
	{
		static T get(Args<CharT>& args)
		{
			throw ArgType::Variant;
		}
	};

private:
	template <typename CharT, typename T, size_t I>
	struct VariantDescription
	{
		static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
		{
			return get_type_description<std::variant_alternative_t<I - 1, T>, CharT>(enum_info) + _ARGLESS_CHAR(CharT, '|') + VariantDescription<CharT, T, I - 1>::get_simple_type_description(enum_info);
		}
	};

	template <typename CharT, typename T>
	struct VariantDescription<CharT, T, 1>
	{
		static std::basic_string<CharT> get_simple_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
		{
			return get_type_description<std::variant_alternative_t<0, T>, CharT>(enum_info);
		}
	};
};

_ARGLESS_PRIVATE_END
