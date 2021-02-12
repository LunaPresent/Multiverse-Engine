/**
	\todo fix potential loss of data in magnitude return value
*/

#pragma once
#include "setup.h"

#include <cstddef> // nullptr_t
#include <type_traits> // enable_if
#include <ostream> // ostream
#include <utility> // declval

namespace mv
{
#pragma warning(push)
#pragma warning(disable:4201)
	namespace detail
	{
		template <typename T, unsigned int N>
		struct VectorBase
		{
			union
			{
				struct { T x, y, z, w; };
				struct { T r, g, b, a; };
				T components[N];
			};

			template <typename... V>
			VectorBase(V&&... components);
		};

		template <typename T>
		struct VectorBase<T, 1>
		{
			union
			{
				struct { T x; };
				struct { T r; };
				T components[1];
			};

			VectorBase(T x = T{});
		};

		template <typename T>
		struct VectorBase<T, 2>
		{
			union
			{
				struct { T x, y; };
				struct { T r, g; };
				T components[2];
			};

			VectorBase(T x = T{}, T y = T{});
		};

		template <typename T>
		struct VectorBase<T, 3>
		{
			union
			{
				struct { T x, y, z; };
				struct { T r, g, b; };
				T components[3];
			};

			VectorBase(T x = T{}, T y = T{}, T z = T{});
		};
	};
#pragma warning(pop)

	enum : bool
	{
		ROW,
		COLUMN
	};

	template <typename T, unsigned int N, bool D = mv::COLUMN>
	class Vector : public detail::VectorBase<T, N>
	{
		static_assert(T{ -1 } < T{ 0 }, "Vector component type must be a signed type");

	public:
		using value_type = T;
		static constexpr unsigned int dimension = N;

	public:
		/**
			\brief default constructor

			zero initialises the vector
		*/
		Vector();
		Vector(const Vector<T, N, D>& obj) = default;
		// constructor for braced enclosed initializer list. unspecified values are zero-initiated
		template <typename... V>
		Vector(V&&... components); // allows for compile-time bounds checking of the list
		// constructor for extending a vector
		template <unsigned int _N, typename... V>
		Vector(const Vector<T, _N, D>& vector, V&&... components);
		// constructor for appending two vectors
		template <unsigned int _N>
		Vector(const Vector<T, _N, D>& vec0, const Vector<T, N - _N, D>& vec1);
		// constructor for type-casting a vector
		template <typename _T>
		Vector(const Vector<_T, N>& vec);
		// constructor for size-casting a vector
		template <unsigned int _N>
		Vector(const Vector<T, _N>& vec);
		// constructor for size- and type-casting a vector
		template <typename _T, unsigned int _N>
		Vector(const Vector<_T, _N>& vec);

		Vector& operator=(const Vector<T, N, D>& obj) = default;

		// generators
		/**
			\brief generate vector basis
		*/
		static const Vector<T, N, D>* basis();


		// accessors
		value_type& operator[](unsigned int i);
		const value_type& operator[](unsigned int i) const;
		value_type& at(unsigned int i);
		const value_type& at(unsigned int i) const;


		const Vector<T, N, !D>& transpose() const;
		Vector<T, N, D> normalise() const;

		double angle(const Vector<T, N, D>& reference) const;
		double magnitude() const;
		value_type squared_magnitude() const;

		value_type dot(const Vector<T, N, D>& rhs) const;
		template <typename... V>
		typename std::enable_if<sizeof...(V) == N - 2, Vector<T, N, D>>::type cross(const Vector<V, N, D>&... ref_vecs) const;


		bool operator==(const Vector<T, N, D> rhs) const;
		bool operator!=(const Vector<T, N, D> rhs) const;


		Vector<T, N, D> operator-() const;

		template <typename _T>
		Vector<T, N, D>& operator+=(const Vector<_T, N, D>& rhs);
		template <typename _T>
		Vector<T, N, D> operator+(const Vector<_T, N, D>& rhs) const;

		template <typename _T>
		Vector<T, N, D>& operator-=(const Vector<_T, N, D>& rhs);
		template <typename _T>
		Vector<T, N, D> operator-(const Vector<_T, N, D>& rhs) const;

		template <typename _T>
		Vector<T, N, D>& operator*=(const _T& rhs);
		template <typename _T>
		Vector<T, N, D> operator*(const _T& rhs) const;

		template <typename _T>
		Vector<T, N, D>& operator/=(const _T& rhs);
		template <typename _T>
		Vector<T, N, D> operator/(const _T& rhs) const;

	private:
		static const Vector<T, N, D>* _cache_init_basis();
	};

	template <typename _T, typename T, unsigned int N, bool D>
	Vector<T, N, D> operator*(const _T& lhs, const Vector<T, N, D>& rhs);

	template <typename TL, typename TR, unsigned int N>
	decltype(std::declval<TL>() * std::declval<TR>()) operator*(const Vector<TL, N, ROW>& lhs, const Vector<TR, N, COLUMN>& rhs);

	template <typename T, unsigned int N, bool D>
	std::ostream& operator<<(std::ostream& stream, const Vector<T, N, D>& vec);


#pragma region aliases
	template <typename T, unsigned int N>
	using rvec = Vector<T, N, ROW>;
	template <typename T, unsigned int N>
	using cvec = Vector<T, N, COLUMN>;
	template <typename T, unsigned int N>
	using vec = Vector<T, N>;


	template <typename T>
	using rvec1 = rvec<T, 1>;
	template <typename T>
	using rvec2 = rvec<T, 2>;
	template <typename T>
	using rvec3 = rvec<T, 3>;
	template <typename T>
	using rvec4 = rvec<T, 4>;

	using rvec1f = rvec1<float>;
	using rvec2f = rvec2<float>;
	using rvec3f = rvec3<float>;
	using rvec4f = rvec4<float>;

	using rvec1d = rvec1<double>;
	using rvec2d = rvec2<double>;
	using rvec3d = rvec3<double>;
	using rvec4d = rvec4<double>;

	using rvec1b = rvec1<std::int8_t>;
	using rvec2b = rvec2<std::int8_t>;
	using rvec3b = rvec3<std::int8_t>;
	using rvec4b = rvec4<std::int8_t>;

	using rvec1s = rvec1<std::int16_t>;
	using rvec2s = rvec2<std::int16_t>;
	using rvec3s = rvec3<std::int16_t>;
	using rvec4s = rvec4<std::int16_t>;

	using rvec1i = rvec1<std::int32_t>;
	using rvec2i = rvec2<std::int32_t>;
	using rvec3i = rvec3<std::int32_t>;
	using rvec4i = rvec4<std::int32_t>;

	using rvec1l = rvec1<std::int64_t>;
	using rvec2l = rvec2<std::int64_t>;
	using rvec3l = rvec3<std::int64_t>;
	using rvec4l = rvec4<std::int64_t>;


	template <typename T>
	using cvec1 = cvec<T, 1>;
	template <typename T>
	using cvec2 = cvec<T, 2>;
	template <typename T>
	using cvec3 = cvec<T, 3>;
	template <typename T>
	using cvec4 = cvec<T, 4>;

	using cvec1f = cvec1<float>;
	using cvec2f = cvec2<float>;
	using cvec3f = cvec3<float>;
	using cvec4f = cvec4<float>;

	using cvec1d = cvec1<double>;
	using cvec2d = cvec2<double>;
	using cvec3d = cvec3<double>;
	using cvec4d = cvec4<double>;

	using cvec1b = cvec1<std::int8_t>;
	using cvec2b = cvec2<std::int8_t>;
	using cvec3b = cvec3<std::int8_t>;
	using cvec4b = cvec4<std::int8_t>;

	using cvec1s = cvec1<std::int16_t>;
	using cvec2s = cvec2<std::int16_t>;
	using cvec3s = cvec3<std::int16_t>;
	using cvec4s = cvec4<std::int16_t>;

	using cvec1i = cvec1<std::int32_t>;
	using cvec2i = cvec2<std::int32_t>;
	using cvec3i = cvec3<std::int32_t>;
	using cvec4i = cvec4<std::int32_t>;

	using cvec1l = cvec1<std::int64_t>;
	using cvec2l = cvec2<std::int64_t>;
	using cvec3l = cvec3<std::int64_t>;
	using cvec4l = cvec4<std::int64_t>;


	template <typename T>
	using vec1 = vec<T, 1>;
	template <typename T>
	using vec2 = vec<T, 2>;
	template <typename T>
	using vec3 = vec<T, 3>;
	template <typename T>
	using vec4 = vec<T, 4>;

	using vec1f = vec1<float>;
	using vec2f = vec2<float>;
	using vec3f = vec3<float>;
	using vec4f = vec4<float>;

	using vec1d = vec1<double>;
	using vec2d = vec2<double>;
	using vec3d = vec3<double>;
	using vec4d = vec4<double>;

	using vec1b = vec1<std::int8_t>;
	using vec2b = vec2<std::int8_t>;
	using vec3b = vec3<std::int8_t>;
	using vec4b = vec4<std::int8_t>;

	using vec1s = vec1<std::int16_t>;
	using vec2s = vec2<std::int16_t>;
	using vec3s = vec3<std::int16_t>;
	using vec4s = vec4<std::int16_t>;

	using vec1i = vec1<std::int32_t>;
	using vec2i = vec2<std::int32_t>;
	using vec3i = vec3<std::int32_t>;
	using vec4i = vec4<std::int32_t>;

	using vec1l = vec1<std::int64_t>;
	using vec2l = vec2<std::int64_t>;
	using vec3l = vec3<std::int64_t>;
	using vec4l = vec4<std::int64_t>;
#pragma endregion
}

#include "Vector.inl"
