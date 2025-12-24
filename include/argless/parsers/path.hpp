#pragma once
#include "../parser.hpp"

#if defined(ARGLESS_STDH_FILESYSTEM) || defined(_GLIBCXX_EXPERIMENTAL_FS_PATH_H) || defined(_GLIBCXX_FILESYSTEM) || defined(_LIBCPP_FILESYSTEM) || defined(_FILESYSTEM_)

#include <filesystem>

_ARGLESS_CORE_BEGIN

template <>
struct parser<std::filesystem::path>
{
	using type = std::filesystem::path; 

	template <typename char_t>
	static inline parse_result<type, char_t> parse(args<char_t>& args)
	{
		auto arg = args.peak();
		if (!arg)
			return make_expected<type>; 
		auto& value = *arg;

		auto result = [&]() -> parse_result<type, char_t>
		{
			try
			{
				return type(value, value + slen(value));
			}
			catch (...)
			{
				return make_expected<type>;
			}
		}();

		if (result.is_valid())
			args.consume();

		return result;
	}

	template <typename char_t>
	static constexpr inline auto name = str_from<char_t, "path">();
};

_ARGLESS_CORE_END

#endif
