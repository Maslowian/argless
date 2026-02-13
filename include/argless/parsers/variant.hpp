#pragma once
#include "../parser.hpp"

#if defined(ARGLESS_STDH_VARIANT) || defined(_GLIBCXX_VARIANT) || defined(_LIBCPP_VARIANT) || defined(_VARIANT_)

#include <variant>

_ARGLESS_CORE_BEGIN

template <>
struct parser<std::variant<std::monostate>>
{
	using type = std::variant<std::monostate>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		return make_default(type());
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t, "<>">();
};

template <typename... var_ts>
struct parser<std::variant<var_ts...>>
{
	static_assert(sizeof...(var_ts) >= 1, "invalid std::variant type");

	using type = std::variant<var_ts...>; 

	using ts = typename tetter<var_ts...>::template filter_t<std::monostate>;

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		return ts::pop_front::invoke_pipe([&]<typename t, std::size_t>(parse_result<type, char_t>&& result) -> parse_result<type, char_t> {
			if (result.is_valid_no_default())
				return result;

			auto next_result = parser<t>::parse(args);

			if (!next_result.is_valid())
				return result;

			if (next_result.is_default())
			{
				if (result.is_valid())
					return result;
				else
					return make_default(type(std::move(next_result).get())); 
			}
			else
				return type(std::move(next_result).get());
		}, [&]() -> parse_result<type, char_t> {
			using t = typename ts::front;
			auto result = parser<t>::parse(args);

			if (!result.is_valid())
				return make_expected<type>;

			if (result.is_default())
				return make_default(type(std::move(result).get()));
			else
				return type(std::move(result).get());
		}());
	}

	template <typename char_t>
	static constexpr inline auto name = []() {
		auto str = ts::pop_front::invoke_pipe([]<typename t>(auto str){ return [](){
				return or_name<std::remove_cvref_t<decltype(str)>{}(), type_name<t, char_t>()>(); 
			}; }, [](){
				return type_name<typename ts::front, char_t>();
			});
		if constexpr (tetter<var_ts...>::template find_t<std::monostate>::value) return optional_name<char_t, decltype(str){}()>();
		else return str();
	}();
};

_ARGLESS_CORE_END

#endif
