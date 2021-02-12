#pragma once
#include "setup.h"

#include <tuple>

namespace mv
{
	template <typename T, typename... V>
	struct IndexOf;
	template <typename T, typename V1, typename... V>
	struct IndexOf<T, V1, V...> : public std::integral_constant<uint, IndexOf<T, V...>::value + 1u> {};
	template <typename T, typename... V>
	struct IndexOf<T, T, V...> : public std::integral_constant<uint, 0u> {};
	template <typename T>
	struct IndexOf<T> : public std::integral_constant<uint, 0u> {};
}
