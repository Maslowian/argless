#pragma once
#include "../parser.hpp"

#if defined(ARGLESS_STDH_OPTIONAL) || defined(_GLIBCXX_OPTIONAL) || defined(_LIBCPP_OPTIONAL) || defined(_OPTIONAL_)

#include <optional>

_ARGLESS_CORE_BEGIN

template <typename opt_t>
struct parser<std::optional<opt_t>>
{
	using type = std::optional<opt_t>; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto result = parser<opt_t>::parse(args);

		if (!result.is_valid())
			return make_default(type());

		return result;
	}

	template <typename char_t>
	static constexpr inline auto name = optional_name<type_name<opt_t, char_t>()>();
};

_ARGLESS_CORE_END

#endif
