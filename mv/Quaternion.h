#pragma once
#include "Vector.h"

namespace mv
{
	template <typename T>
	class Quaternion
	{
	public:
		T x, y, z, w;

		Quaternion();
		template <bool D>
		Quaternion(const Vector<T, 3, D>& euler_angles);
		Quaternion(T yaw, T pitch, T roll);
		template <bool D>
		Quaternion(const Vector<T, 3, D>& axis, T angle);
	};

#pragma region aliases
	template <typename T>
	using quat = Quaternion<T>;

	using quatf = quat<float>;
	using quatd = quat<double>;
#pragma endregion
}

#include "Quaternion.inl"
