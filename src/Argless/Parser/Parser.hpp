#pragma once
#include <functional>
#include <string>
#include "../Core.hpp"
#include "../Args.hpp"

_ARGLESS_PRIVATE_BEGIN

template <typename CharT, typename T>
constexpr std::function<T(Args<CharT>&)> get_parser();

template <typename T, typename CharT>
std::basic_string<CharT> get_type_description(const std::function<std::vector<std::basic_string<CharT>>(size_t)>& enum_info);

_ARGLESS_PRIVATE_END
