#pragma once
#include "../parser.hpp"
#include <type_traits>

_ARGLESS_BEGIN

template <_ARGLESS_CORE str, auto value_, bool>
	requires std::is_enum_v<decltype(value_)>
struct enum_value;

_ARGLESS_END
_ARGLESS_CORE_BEGIN

template <typename t>
struct is_enum_value : public std::false_type {};

template <auto name, auto value, bool nocase>
struct is_enum_value<enum_value<name, value, nocase>> : public std::true_type {};

template <typename t>
concept enum_value_t = is_enum_value<t>::value;

template <typename... values>
concept no_enum_name_collision = tetter<values...>::template value_l<[]<typename t, std::size_t i>() {
		return tetter<values...>::template pop_front_n<i + 1>::template value_l<[]<typename st>() {
			if constexpr (t::nocase || st::nocase)
				return _ARGLESS_CORE seq_nocase(t::name.data(), st::name.data()); 
			else
				return _ARGLESS_CORE seq(t::name.data(), st::name.data()); 
		}>::any;
	}>::none;

_ARGLESS_CORE_END




_ARGLESS_BEGIN

template <_ARGLESS_CORE str name_, auto value_, bool nocase_ = false>
	requires std::is_enum_v<decltype(value_)>
struct enum_value
{
	static constexpr auto name = name_;
	static constexpr auto value = value_;
	static constexpr auto nocase = nocase_;
};

template <_ARGLESS_CORE enum_value_t... values_>
	requires (sizeof...(values_) > 0) && _ARGLESS_CORE no_enum_name_collision<values_...>
struct enum_values
{
	using values = tetter<values_...>;
};

// Example specialization:
/*
template <>
struct argless::enum_refl<ur_enum>
{
	using values = argless::enum_values<
		argless::enum_value<"name", ur_enum::value>, ...
	>;
};
*/
template <typename t>
struct enum_refl;

_ARGLESS_END



_ARGLESS_CORE_BEGIN

template <typename t>
struct is_scoped_enum : std::integral_constant<bool, std::is_enum_v<t> && !std::is_convertible_v<t, std::underlying_type_t<t>>> {};

template <typename t>
struct is_enum_values : public std::false_type {};

template <typename... ts>
struct is_enum_values<enum_values<ts...>> : public std::true_type {};

template <typename enum_t>
	requires std::is_enum_v<enum_t>
struct parser<enum_t>
{
	static_assert(requires { typename enum_refl<enum_t>::values; } &&
			is_enum_values<typename enum_refl<enum_t>::values>::value &&
			enum_refl<enum_t>::values::values::template value_l<[]<typename t, std::size_t>(){ return std::is_same_v<std::remove_cvref_t<decltype(t::value)>, enum_t>; }>::all, 
			"this enum is not specialized (use argless::enum_refl) or used wrong type for ::values (must be argless::enum_values) or u used diffrent enum type in argless::enum_value");

	using type = enum_t; 

	using vs = typename enum_refl<enum_t>::values::values;

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		auto& value = *arg;

		auto result = vs::pop_front::invoke_pipe([&]<typename t>(parse_result<type, char_t>&& result) -> parse_result<type, char_t> {
			if (result.is_valid()) return result;
			if([&](){
				if constexpr (t::nocase)
					return seq_nocase(t::name.data(), value); 
				else
					return seq(t::name.data(), value); 
			}())
				return args.consume(), t::value;
			else
				return result;
		}, [&]() -> parse_result<type, char_t> {
			using t = typename vs::front;
			if([&](){
				if constexpr (t::nocase)
					return seq_nocase(t::name.data(), value); 
				else
					return seq(t::name.data(), value); 
			}())
				return args.consume(), t::value;
			else
				return make_expected<type>;
		}());

		if constexpr (!is_scoped_enum<enum_t>::value)
		{
			if (!result.is_valid())
			{
				auto u_result = parser<std::underlying_type_t<enum_t>>::parse(args);

				if (u_result.is_valid())
					return u_result;
			}
		}

		return result;
	}

	template <typename char_t>
	static constexpr inline auto name = [](){
		auto str = vs::pop_front::invoke_pipe([]<typename t>(auto str){ return [](){ return or_name<decltype(str){}(), (str_from<char_t, "'">() + str_cast<char_t, t::name>() + str_from<char_t, "'">())>(); }; },
				[](){ return str_from<char_t, "'">() + str_cast<char_t, vs::front::name>() + str_from<char_t, "'">(); });
		if constexpr (!is_scoped_enum<enum_t>::value)
			return or_name<decltype(str){}(), str_name<std::underlying_type_t<enum_t>, char_t>()>(); 
		else
			return str();
	}();
};

_ARGLESS_CORE_END
