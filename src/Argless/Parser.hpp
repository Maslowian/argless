#pragma once
#include "Parser/FundamentalType.hpp"
#include "Parser/Array.hpp"
#include "Parser/DynamicArray.hpp"
#include "Parser/Tuple.hpp"
#include "Parser/Enum.hpp"
#include "Parser/String.hpp"
#include "Parser/Optional.hpp"
#include "Parser/RuleString.hpp"
#include "Parser/Variant.hpp"
#include "Parser/Path.hpp"

_ARGLESS_PRIVATE_BEGIN

#define _ARGLESS_PARSERS \
	FundamentalType, \
	Array, \
	DynamicArray, \
	Tuple, \
	Enum, \
	String, \
	Optional, \
	Variant, \
	RuleString, \
	Path

template <typename... Types>
struct GetSize 
{
	static constexpr inline size_t value = std::tuple_size_v<std::tuple<Types...>>;
};

template <size_t I, typename... Types>
struct GetType 
{
	using type = std::tuple_element_t<I, std::tuple<Types...>>;
};

template <size_t I, typename T, bool Succeed, typename... Parsers>
struct GetParser
{
private:
	using CurrentParser = typename GetType<I - 1, Parsers...>::type;
	static constexpr inline bool Success = CurrentParser::template is_t<T>::value;
	using NextParser = typename GetParser<I - 1, T, Success || Succeed, Parsers...>::type;
public:
	using type = std::conditional_t<Success, CurrentParser, NextParser>;
};

template <typename T, bool Succeed, typename... Parsers>
struct GetParser<0, T, Succeed, Parsers...>
{
	_ARGLESS_STATIC_ASSERT(Succeed, "Type not supported");
	using type = void;
};

template <typename CharT, typename T>
constexpr std::function<T(Args<CharT>&)> get_parser()
{
	return std::function<T(Args<CharT>&)>(&GetParser<GetSize<_ARGLESS_PARSERS>::value, T, false, _ARGLESS_PARSERS>::type::template get<CharT, T>);
}

template <typename T, typename CharT>
std::basic_string<CharT> get_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info)
{
	return GetParser<GetSize<_ARGLESS_PARSERS>::value, T, false, _ARGLESS_PARSERS>::type::template get_description<T, CharT>(enum_info);
}


_ARGLESS_PRIVATE_END
