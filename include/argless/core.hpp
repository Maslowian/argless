#pragma once

#define ARGLESS_VERSION_MAJOR 2
#define ARGLESS_VERSION_MINOR 0
#define ARGLESS_VERSION_PATCH 0

#define _ARGLESS_BEGIN namespace argless {
#define _ARGLESS_END }
#define _ARGLESS ::argless::

#define _ARGLESS_CORE_BEGIN _ARGLESS_BEGIN namespace core {
#define _ARGLESS_CORE_END _ARGLESS_END }
#define _ARGLESS_CORE _ARGLESS core::

#ifndef ARGLESS_ASSERT
#include <cassert>
#define ARGLESS_ASSERT(expr, msg) do { assert((expr) && msg); } while (0)
#endif
