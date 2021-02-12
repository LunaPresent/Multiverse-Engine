#include "Quaternion.h"

#include <cmath>


template<typename T>
mv::Quaternion<T>::Quaternion()
	: x{ 0 }, y{ 0 }, z{ 0 }, w{ 1 }
{}

template <typename T>
template <bool D>
mv::Quaternion<T>::Quaternion(const Vector<T, 3, D>& euler_angles)
	: Quaternion(euler_angles.x, euler_angles.y, euler_angles.z)
{}

template <typename T>
mv::Quaternion<T>::Quaternion(T yaw, T pitch, T roll)
{
	T cy = std::cos(yaw / 2);
	T cp = std::cos(pitch / 2);
	T cr = std::cos(roll / 2);
	T sy = std::sin(yaw / 2);
	T sp = std::sin(pitch / 2);
	T sr = std::sin(roll / 2);

	this->x = sy * cp * cr - cy * sp * sr;
	this->y = cy * sp * cr + sy * cp * sr;
	this->z = cy * cp * sr - sy * sp * cr;
	this->w = cy * cp * cr + sy * sp * sr;
}

template <typename T>
template <bool D>
mv::Quaternion<T>::Quaternion(const Vector<T, 3, D>& axis, T angle)
{
	T sin = std::sin(angle / 2);
	this->x = axis.x * sin;
	this->y = axis.y * sin;
	this->z = axis.z * sin;
	this->w = std::cos(angle / 2);
}

