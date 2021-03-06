#pragma once
#include <tuple>

namespace mv
{
	template <typename T, typename... V>
	struct IndexOf;
	template <typename T, typename V1, typename... V>
	struct IndexOf<T, V1, V...> : public std::integral_constant<unsigned int, IndexOf<T, V...>::value + 1u> {};
	template <typename T, typename... V>
	struct IndexOf<T, T, V...> : public std::integral_constant<unsigned int, 0u> {};
	template <typename T>
	struct IndexOf<T> : public std::integral_constant<unsigned int, 0u> {};

	template <unsigned int n>
	struct log2 : public std::integral_constant<unsigned int, log2<n / 2u>::value + 1u> {};
	template <>
	struct log2<1u> : public std::integral_constant<unsigned int, 0u> {};
	template <>
	struct log2<0u>;
}
