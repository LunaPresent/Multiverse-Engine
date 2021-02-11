#pragma once
#include <cstddef>

namespace mv
{
	using int8 = signed char;
	using int16 = short;
	using int32 = int;
	using int64 = long long;
	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	using uint64 = unsigned long long;

	using byte = uint8;
	using uint = uint32;

	using id_type = uint;
	using size_type = uint;

	constexpr id_type invalid_id = static_cast<id_type>(-1);
	constexpr float pi = 3.14159265f;
	constexpr uint module_id_bits = 8;
}
