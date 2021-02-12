#pragma once
#include "setup.h"
#include "Vector.h" // Vector

#include <cstddef>	// nullptr_t
#include <type_traits> // enable_if
#include <ostream> // ostream
#include <utility> // declval

namespace mv
{
	/**
		\brief enum to specify matrix as row- or column major

		this enum logically only has two states, hence the underlying type is bool
		this makes it so that the state can be inverted using !state
	*/

	template <typename T, unsigned int R, unsigned int C, bool D = mv::ROW>
	class Matrix
	{
	public:
		static constexpr unsigned int row_count = R;
		static constexpr unsigned int column_count = C;
		static constexpr bool data_order = D;
		static constexpr unsigned int size = R * C;
		static constexpr unsigned int outer_dimension = (data_order == ROW ? row_count : column_count);
		static constexpr unsigned int inner_dimension = (data_order == ROW ? column_count : row_count);

		using value_type = T;
		using vector_type = Vector<value_type, inner_dimension, data_order>;
		using row_type = Vector<value_type, column_count, ROW>;
		using column_type = Vector<value_type, row_count, COLUMN>;

		static_assert(size / row_count == column_count, "matrix size overflow");

	private:
		union {
			value_type _elements[size];
			vector_type _vectors[outer_dimension];
		};

	public:
		/**
			\brief default constructor

			initialises a zero matrix
		*/
		Matrix();

		/**
			\brief constructor for list of elements

			elements are ordered the same as their order inside the matrix
			e.g. for Matrix<T, 2, 4, COLUMN> the first 4 elements
			will represent the first column, for Matrix<T, 2, 4, ROW>
			the first 4 elements represent the first 2 rows
		*/
		template <typename... V>
		Matrix(const V&... elements);

		/**
			\brief constructor for list of vectors

			vectors represent the major dimension of the matrix
		*/
		template <typename... V>
		Matrix(const Vector<V, mv::Matrix<T, R, C, D>::inner_dimension, D>&... vectors);

		/**
		*/
		template <typename... V>
		Matrix(const Vector<V, mv::Matrix<T, R, C, D>::outer_dimension, !D>&... vectors);

		/**
			\brief constructor for transposing data representation
		*/
		Matrix(const Matrix<T, R, C, !D>& matrix);


		template <unsigned int _R = row_count>
		operator typename std::enable_if<_R == 1, row_type>::type() const;
		template <unsigned int _C = column_count>
		operator typename std::enable_if<_C == 1, column_type>::type() const;


		// generators
		/**
			\brief generate identity matrix

			square matrix types only
			generates the identity matrix of the RxC matrix space
		*/
		template <unsigned int _R = row_count, unsigned int _C = column_count>
		static typename std::enable_if<_R == _C, const Matrix<T, R, C, D>&>::type identity();

		/**
			\brief generate translation matrix (row vector)
		*/
		template <unsigned int _R = row_count, unsigned int _C = column_count>
		static typename std::enable_if<_R == _C, Matrix<T, R, C, D>>::type translate(
			const Vector<T, mv::Matrix<T, R, C, D>::column_count - 1, ROW>& translation_row_vector);
		/**
			\brief generate translation matrix (column vector)
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == _C, Matrix<T, R, C, D>>::type translate(
			const Vector<T, mv::Matrix<T, R, C, D>::row_count - 1, COLUMN>& translation_column_vector);

		/**
			\brief generate scale matrix
		*/
		template <bool _D, unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == _C, Matrix<T, R, C, D>>::type scale(
			const Vector<T, mv::Matrix<T, R, C, D>::outer_dimension - 1, _D>& scale_vector);
		/**
			\brief generate uniform scale matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == _C, Matrix<T, R, C, D>>::type scale(const T& uniform_scalar);

		/**
			\brief generate 2D rotation matrix
		*/
		template <bool _D = mv::ROW, unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type rotate(double angle);
		/**
			\brief generate 3D rotation matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 4 && _C == 4, Matrix<T, R, C, D>>::type rotate(
			double angle,
			const Vector<T, 3, D>& rotation_axis);
		/**
			\brief generate 3D rotation matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 4 && _C == 4, Matrix<T, R, C, D>>::type rotate(
			double angle,
			const Vector<T, 3, !D>& rotation_axis);
		/**
			\brief generate rotation matrix (row vector)

			this function expects 2 orthonormal vectors as input
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == _C && _R >= 3, Matrix<T, R, C, D>>::type rotate(
			double angle,
			const Vector<T, mv::Matrix<T, R, C, D>::outer_dimension - 1, ROW>& axis_vector,
			const Vector<T, mv::Matrix<T, R, C, D>::outer_dimension - 1, ROW>& direction_vector);

		/**
			\brief generate 2D transformation matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type transform(
			const Vector<T, 2, D>& translation_vector,
			double angle,
			const Vector<T, 2, D>& scale_vector);
		/**
			\brief generate 2D transformation matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type transform(
			const Vector<T, 2, D>& translation,
			double angle,
			const T& uniform_scalar);
		/**
			\brief generate 2D transformation matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type transform(
			const Vector<T, 2, !D>& translation_vector,
			double angle,
			const Vector<T, 2, !D>& scale_vector);
		/**
			\brief generate 2D transformation matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type transform(
			const Vector<T, 2, !D>& translation,
			double angle,
			const T& uniform_scalar);

		/**
			\brief generate 2D viewpoint matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type viewpoint(
			const Vector<T, 2, D>& position_vector,
			const Vector<T, 2, D>& up_unit_vector);
		/**
			\brief generate 2D viewpoint matrix
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 3 && _C == 3, Matrix<T, R, C, D>>::type viewpoint(
			const Vector<T, 2, !D>& position_vector,
			const Vector<T, 2, !D>& up_unit_vector);
		/**
			\brief generate 3D viewpoint matrix
			\param position_vector the viewpoint's origin
			\param direction_unit_vector a unit vector pointing from the target towards the viewpoint's origin
			\param up_axis_vector a vector denoting the up axis of the view space

			this function requires that direction_unit_vector is not equal or opposite to up_axis_vector.normalise()
			e.g. the camera cannot be pointed straight down or straight up, the viewpoint overload which takes a
			relative up unit vector can be used instead
		*/
		template <bool _D, unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 4 && _C == 4, Matrix<T, R, C, D>>::type viewpoint(
			const Vector<T, 3, _D>& position_vector,
			const Vector<T, 3, _D>& direction_unit_vector,
			const Vector<T, 3, _D>& up_axis_vector);
		/**
			\brief generate 3D viewpoint matrix
			\param position_vector the viewpoint's origin
			\param direction_unit_vector a unit vector pointing from the target towards the viewpoint's origin
			\param up_unit_vector a unit vector orthogonal to direction_unit_vector pointing towards the viewpoint's relative up
			\param right_unit_vector a unit vector equal to (up_unit_vector x direction_unit_vector)
		*/
		template <bool _D, unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 4 && _C == 4, Matrix<T, R, C, D>>::type viewpoint(
			const Vector<T, 3, _D>& position_vector,
			const Vector<T, 3, _D>& direction_unit_vector,
			const Vector<T, 3, _D>& up_unit_vector,
			const Vector<T, 3, _D>& right_unit_vector);

		/**
			\brief generate 3D perspective matrix
		*/
		template <bool _D = mv::ROW, unsigned int _R = R, unsigned int _C = C>
		static typename std::enable_if<_R == 4 && _C == 4, Matrix<T, 4, 4>>::type perspective(
			value_type view_angle, value_type aspect_ratio, value_type near_plane, value_type far_plane);

		/*	template <unsigned int _R = R, unsigned int _C = C>
			static typename std::enable_if<_R == 4 && _C == 4, Matrix<T, 4, 4>>::type ortho(double width, double height, double near, double far);*/

		// accessors/mutators
		/**

		*/
		vector_type& operator[](unsigned int i);
		const vector_type& operator[](unsigned int i) const;
		vector_type& at(unsigned int i);
		const vector_type& at(unsigned int i) const;
		value_type& operator()(unsigned int row_index, unsigned int column_index);
		const value_type& operator()(unsigned int row_index, unsigned int column_index) const;
		value_type& at(unsigned int row_index, unsigned int column_index);
		const value_type& at(unsigned int row_index, unsigned int column_index) const;

		template <bool _D = D>
		typename std::enable_if<_D == ROW, const row_type&>::type get_row(unsigned int row_index) const;
		template <bool _D = D>
		typename std::enable_if<_D == COLUMN, row_type>::type get_row(unsigned int row_index) const;
		template <bool _D = D>
		typename std::enable_if<_D == COLUMN, const column_type&>::type get_column(unsigned int column_index) const;
		template <bool _D = D>
		typename std::enable_if<_D == ROW, column_type>::type get_column(unsigned int column_index) const;

		template <bool _D = D>
		typename std::enable_if<_D == ROW, void>::type set_row(unsigned int row_index, const row_type& row);
		template <bool _D = D>
		typename std::enable_if<_D == COLUMN, void>::type set_row(unsigned int row_index, const row_type& row);
		template <bool _D = D>
		typename std::enable_if<_D == COLUMN, void>::type set_column(unsigned int column_index, const column_type& column);
		template <bool _D = D>
		typename std::enable_if<_D == ROW, void>::type set_column(unsigned int column_index, const column_type& column);

		/**
			\brief calculate determinant
			\complexity exponential in matrix size

			square matrix types only
			calculates the determinant of the matrix
			this function wil produce the same result regardless of data order
			(matrix.determinant() = matrix.transpose().determinant())
		*/
		template <unsigned int _R = R, unsigned int _C = C>
		typename std::enable_if<_R == _C, value_type>::type determinant() const;

		/**
			\brief matrix transpose

			returns the transpose of a matrix
			this operation returns a reference to the original matrix
			but of a type that makes it represent its transpose,
			so catching it in a const reference will not copy any data
		*/
		const Matrix<T, C, R, static_cast<bool>(!static_cast<bool>(D))>& transpose() const;

		template <unsigned int _R = R, unsigned int _C = C>
		typename std::enable_if<_R == _C, Matrix<T, R, C, D>>::type inverse() const;

		template <typename _T, bool _D>
		bool operator==(const Matrix<_T, R, C, _D>& rhs) const;
		template <typename _T, bool _D>
		bool operator!=(const Matrix<_T, R, C, _D>& lhs) const;


		/**
			\brief unary negate
		*/
		Matrix<T, R, C, D> operator-() const;

		template <typename _T, bool _D>
		Matrix<T, R, C, D>& operator+=(const Matrix<_T, R, C, _D>& rhs);
		template <typename _T, bool _D>
		Matrix<T, R, C, D> operator+(const Matrix<_T, R, C, _D>& rhs) const;

		template <typename _T, bool _D>
		Matrix<T, R, C, D>& operator-=(const Matrix<_T, R, C, _D>& rhs);
		template <typename _T, bool _D>
		Matrix<T, R, C, D> operator-(const Matrix<_T, R, C, _D>& rhs) const;

		template <typename _T>
		Matrix<T, R, C, D>& operator*=(const _T& rhs);
		template <typename _T>
		Matrix<T, R, C, D> operator*(const _T& rhs) const;

		template <typename _T>
		Matrix<T, R, C, D>& operator/=(const _T& rhs);
		template <typename _T>
		Matrix<T, R, C, D> operator/(const _T& rhs) const;

		template <typename TR, bool DR>
		Matrix<T, R, C, D>& operator*=(const Matrix<TR, C, C, DR>& rhs);
		template <typename TR, unsigned int CR, bool DR>
		Matrix<decltype(std::declval<T>() * std::declval<TR>()), R, CR, D> operator*(const Matrix<TR, C, CR, DR>& rhs) const;
		template <typename TR>
		Vector<decltype(std::declval<T>() * std::declval<TR>()), R, COLUMN> operator*(const Vector<TR, C, COLUMN>& rhs) const;
		template <typename TR, unsigned int CR, unsigned int _C = C>
		typename std::enable_if<_C == 1, Matrix<decltype(std::declval<T>() * std::declval<TR>()), R, CR, D>>::type operator*(const Vector<TR, CR, ROW>& rhs);

	private:
		template <unsigned int _R = row_count, unsigned int _C = column_count>
		static typename std::enable_if<_R == _C, Matrix<T, R, C, D>>::type _cache_init_identity();
	};

	template <typename _T, typename T, unsigned int R, unsigned int C, bool D>
	Matrix<T, R, C, D> operator*(const _T& lhs, const Matrix<T, R, C, D>& rhs);
	/**
		\brief vector outer product

		to get the other matrix representation of v * u, one can use
		transpose(u.transpose() * v.transpose())
		without losing any noticable performance,
		or simply use the _matRx1_ * rvecC_ overload instead
	*/
	template <typename TL, typename TR, unsigned int R, unsigned int C>
	Matrix<decltype(std::declval<TL>() * std::declval<TR>()), R, C> operator*(const Vector<TL, R, COLUMN>& lhs, const Vector<TR, C, ROW>& rhs);
	template <typename TL, typename TR, unsigned int R, unsigned int C, bool D>
	Matrix<decltype(std::declval<TL>() * std::declval<TR>()), R, C, D> operator*(const Vector<TL, R, COLUMN>& lhs, const Matrix<TR, 1, C, D>& rhs);

	template <typename TL, typename TR, unsigned int C, bool D>
	Vector<TL, C, ROW>& operator*=(Vector<TL, C, ROW>& lhs, const Matrix<TR, C, C, D>& rhs);
	template <typename TL, typename TR, unsigned int C, unsigned int CR, bool D>
	Vector<decltype(std::declval<TL>() * std::declval<TR>()), CR, ROW> operator*(const Vector<TL, C, ROW>& lhs, const Matrix<TR, C, CR, D>& rhs);


	template <typename T, unsigned int R, unsigned int C, bool D>
	std::ostream& operator<<(std::ostream& stream, const Matrix<T, R, C, D>& matrix);


#pragma region aliases
	template <typename T, unsigned int R, unsigned int C>
	using rmat = Matrix<T, R, C, ROW>;
	template <typename T, unsigned int R, unsigned int C>
	using cmat = Matrix<T, R, C, COLUMN>;
	template <typename T, unsigned int R, unsigned int C>
	using mat = Matrix<T, R, C>;


	template <typename T>
	using rmat1x1 = rmat<T, 1, 1>;
	template <typename T>
	using rmat1x2 = rmat<T, 1, 2>;
	template <typename T>
	using rmat1x3 = rmat<T, 1, 3>;
	template <typename T>
	using rmat1x4 = rmat<T, 1, 4>;
	template <typename T>
	using rmat2x1 = rmat<T, 2, 1>;
	template <typename T>
	using rmat2x2 = rmat<T, 2, 2>;
	template <typename T>
	using rmat2x3 = rmat<T, 2, 3>;
	template <typename T>
	using rmat2x4 = rmat<T, 2, 4>;
	template <typename T>
	using rmat3x1 = rmat<T, 3, 1>;
	template <typename T>
	using rmat3x2 = rmat<T, 3, 2>;
	template <typename T>
	using rmat3x3 = rmat<T, 3, 3>;
	template <typename T>
	using rmat3x4 = rmat<T, 3, 4>;
	template <typename T>
	using rmat4x1 = rmat<T, 4, 1>;
	template <typename T>
	using rmat4x2 = rmat<T, 4, 2>;
	template <typename T>
	using rmat4x3 = rmat<T, 4, 3>;
	template <typename T>
	using rmat4x4 = rmat<T, 4, 4>;
	template <typename T>
	using rmat1 = rmat1x1<T>;
	template <typename T>
	using rmat2 = rmat2x2<T>;
	template <typename T>
	using rmat3 = rmat3x3<T>;
	template <typename T>
	using rmat4 = rmat4x4<T>;

	using rmat1x1f = rmat1x1<float>;
	using rmat1x2f = rmat1x2<float>;
	using rmat1x3f = rmat1x3<float>;
	using rmat1x4f = rmat1x4<float>;
	using rmat2x1f = rmat2x1<float>;
	using rmat2x2f = rmat2x2<float>;
	using rmat2x3f = rmat2x3<float>;
	using rmat2x4f = rmat2x4<float>;
	using rmat3x1f = rmat3x1<float>;
	using rmat3x2f = rmat3x2<float>;
	using rmat3x3f = rmat3x3<float>;
	using rmat3x4f = rmat3x4<float>;
	using rmat4x1f = rmat4x1<float>;
	using rmat4x2f = rmat4x2<float>;
	using rmat4x3f = rmat4x3<float>;
	using rmat4x4f = rmat4x4<float>;
	using rmat1f = rmat1x1f;
	using rmat2f = rmat2x2f;
	using rmat3f = rmat3x3f;
	using rmat4f = rmat4x4f;

	using rmat1x1d = rmat1x1<double>;
	using rmat1x2d = rmat1x2<double>;
	using rmat1x3d = rmat1x3<double>;
	using rmat1x4d = rmat1x4<double>;
	using rmat2x1d = rmat2x1<double>;
	using rmat2x2d = rmat2x2<double>;
	using rmat2x3d = rmat2x3<double>;
	using rmat2x4d = rmat2x4<double>;
	using rmat3x1d = rmat3x1<double>;
	using rmat3x2d = rmat3x2<double>;
	using rmat3x3d = rmat3x3<double>;
	using rmat3x4d = rmat3x4<double>;
	using rmat4x1d = rmat4x1<double>;
	using rmat4x2d = rmat4x2<double>;
	using rmat4x3d = rmat4x3<double>;
	using rmat4x4d = rmat4x4<double>;
	using rmat1d = rmat1x1d;
	using rmat2d = rmat2x2d;
	using rmat3d = rmat3x3d;
	using rmat4d = rmat4x4d;

	using rmat1x1b = rmat1x1<std::int8_t>;
	using rmat1x2b = rmat1x2<std::int8_t>;
	using rmat1x3b = rmat1x3<std::int8_t>;
	using rmat1x4b = rmat1x4<std::int8_t>;
	using rmat2x1b = rmat2x1<std::int8_t>;
	using rmat2x2b = rmat2x2<std::int8_t>;
	using rmat2x3b = rmat2x3<std::int8_t>;
	using rmat2x4b = rmat2x4<std::int8_t>;
	using rmat3x1b = rmat3x1<std::int8_t>;
	using rmat3x2b = rmat3x2<std::int8_t>;
	using rmat3x3b = rmat3x3<std::int8_t>;
	using rmat3x4b = rmat3x4<std::int8_t>;
	using rmat4x1b = rmat4x1<std::int8_t>;
	using rmat4x2b = rmat4x2<std::int8_t>;
	using rmat4x3b = rmat4x3<std::int8_t>;
	using rmat4x4b = rmat4x4<std::int8_t>;
	using rmat1b = rmat1x1b;
	using rmat2b = rmat2x2b;
	using rmat3b = rmat3x3b;
	using rmat4b = rmat4x4b;

	using rmat1x1s = rmat1x1<std::int16_t>;
	using rmat1x2s = rmat1x2<std::int16_t>;
	using rmat1x3s = rmat1x3<std::int16_t>;
	using rmat1x4s = rmat1x4<std::int16_t>;
	using rmat2x1s = rmat2x1<std::int16_t>;
	using rmat2x2s = rmat2x2<std::int16_t>;
	using rmat2x3s = rmat2x3<std::int16_t>;
	using rmat2x4s = rmat2x4<std::int16_t>;
	using rmat3x1s = rmat3x1<std::int16_t>;
	using rmat3x2s = rmat3x2<std::int16_t>;
	using rmat3x3s = rmat3x3<std::int16_t>;
	using rmat3x4s = rmat3x4<std::int16_t>;
	using rmat4x1s = rmat4x1<std::int16_t>;
	using rmat4x2s = rmat4x2<std::int16_t>;
	using rmat4x3s = rmat4x3<std::int16_t>;
	using rmat4x4s = rmat4x4<std::int16_t>;
	using rmat1s = rmat1x1s;
	using rmat2s = rmat2x2s;
	using rmat3s = rmat3x3s;
	using rmat4s = rmat4x4s;

	using rmat1x1i = rmat1x1<std::int32_t>;
	using rmat1x2i = rmat1x2<std::int32_t>;
	using rmat1x3i = rmat1x3<std::int32_t>;
	using rmat1x4i = rmat1x4<std::int32_t>;
	using rmat2x1i = rmat2x1<std::int32_t>;
	using rmat2x2i = rmat2x2<std::int32_t>;
	using rmat2x3i = rmat2x3<std::int32_t>;
	using rmat2x4i = rmat2x4<std::int32_t>;
	using rmat3x1i = rmat3x1<std::int32_t>;
	using rmat3x2i = rmat3x2<std::int32_t>;
	using rmat3x3i = rmat3x3<std::int32_t>;
	using rmat3x4i = rmat3x4<std::int32_t>;
	using rmat4x1i = rmat4x1<std::int32_t>;
	using rmat4x2i = rmat4x2<std::int32_t>;
	using rmat4x3i = rmat4x3<std::int32_t>;
	using rmat4x4i = rmat4x4<std::int32_t>;
	using rmat1i = rmat1x1i;
	using rmat2i = rmat2x2i;
	using rmat3i = rmat3x3i;
	using rmat4i = rmat4x4i;

	using rmat1x1l = rmat1x1<std::int64_t>;
	using rmat1x2l = rmat1x2<std::int64_t>;
	using rmat1x3l = rmat1x3<std::int64_t>;
	using rmat1x4l = rmat1x4<std::int64_t>;
	using rmat2x1l = rmat2x1<std::int64_t>;
	using rmat2x2l = rmat2x2<std::int64_t>;
	using rmat2x3l = rmat2x3<std::int64_t>;
	using rmat2x4l = rmat2x4<std::int64_t>;
	using rmat3x1l = rmat3x1<std::int64_t>;
	using rmat3x2l = rmat3x2<std::int64_t>;
	using rmat3x3l = rmat3x3<std::int64_t>;
	using rmat3x4l = rmat3x4<std::int64_t>;
	using rmat4x1l = rmat4x1<std::int64_t>;
	using rmat4x2l = rmat4x2<std::int64_t>;
	using rmat4x3l = rmat4x3<std::int64_t>;
	using rmat4x4l = rmat4x4<std::int64_t>;
	using rmat1l = rmat1x1l;
	using rmat2l = rmat2x2l;
	using rmat3l = rmat3x3l;
	using rmat4l = rmat4x4l;


	template <typename T>
	using cmat1x1 = cmat<T, 1, 1>;
	template <typename T>
	using cmat1x2 = cmat<T, 1, 2>;
	template <typename T>
	using cmat1x3 = cmat<T, 1, 3>;
	template <typename T>
	using cmat1x4 = cmat<T, 1, 4>;
	template <typename T>
	using cmat2x1 = cmat<T, 2, 1>;
	template <typename T>
	using cmat2x2 = cmat<T, 2, 2>;
	template <typename T>
	using cmat2x3 = cmat<T, 2, 3>;
	template <typename T>
	using cmat2x4 = cmat<T, 2, 4>;
	template <typename T>
	using cmat3x1 = cmat<T, 3, 1>;
	template <typename T>
	using cmat3x2 = cmat<T, 3, 2>;
	template <typename T>
	using cmat3x3 = cmat<T, 3, 3>;
	template <typename T>
	using cmat3x4 = cmat<T, 3, 4>;
	template <typename T>
	using cmat4x1 = cmat<T, 4, 1>;
	template <typename T>
	using cmat4x2 = cmat<T, 4, 2>;
	template <typename T>
	using cmat4x3 = cmat<T, 4, 3>;
	template <typename T>
	using cmat4x4 = cmat<T, 4, 4>;
	template <typename T>
	using cmat1 = cmat1x1<T>;
	template <typename T>
	using cmat2 = cmat2x2<T>;
	template <typename T>
	using cmat3 = cmat3x3<T>;
	template <typename T>
	using cmat4 = cmat4x4<T>;

	using cmat1x1f = cmat1x1<float>;
	using cmat1x2f = cmat1x2<float>;
	using cmat1x3f = cmat1x3<float>;
	using cmat1x4f = cmat1x4<float>;
	using cmat2x1f = cmat2x1<float>;
	using cmat2x2f = cmat2x2<float>;
	using cmat2x3f = cmat2x3<float>;
	using cmat2x4f = cmat2x4<float>;
	using cmat3x1f = cmat3x1<float>;
	using cmat3x2f = cmat3x2<float>;
	using cmat3x3f = cmat3x3<float>;
	using cmat3x4f = cmat3x4<float>;
	using cmat4x1f = cmat4x1<float>;
	using cmat4x2f = cmat4x2<float>;
	using cmat4x3f = cmat4x3<float>;
	using cmat4x4f = cmat4x4<float>;
	using cmat1f = cmat1x1f;
	using cmat2f = cmat2x2f;
	using cmat3f = cmat3x3f;
	using cmat4f = cmat4x4f;

	using cmat1x1d = cmat1x1<double>;
	using cmat1x2d = cmat1x2<double>;
	using cmat1x3d = cmat1x3<double>;
	using cmat1x4d = cmat1x4<double>;
	using cmat2x1d = cmat2x1<double>;
	using cmat2x2d = cmat2x2<double>;
	using cmat2x3d = cmat2x3<double>;
	using cmat2x4d = cmat2x4<double>;
	using cmat3x1d = cmat3x1<double>;
	using cmat3x2d = cmat3x2<double>;
	using cmat3x3d = cmat3x3<double>;
	using cmat3x4d = cmat3x4<double>;
	using cmat4x1d = cmat4x1<double>;
	using cmat4x2d = cmat4x2<double>;
	using cmat4x3d = cmat4x3<double>;
	using cmat4x4d = cmat4x4<double>;
	using cmat1d = cmat1x1d;
	using cmat2d = cmat2x2d;
	using cmat3d = cmat3x3d;
	using cmat4d = cmat4x4d;

	using cmat1x1b = cmat1x1<std::int8_t>;
	using cmat1x2b = cmat1x2<std::int8_t>;
	using cmat1x3b = cmat1x3<std::int8_t>;
	using cmat1x4b = cmat1x4<std::int8_t>;
	using cmat2x1b = cmat2x1<std::int8_t>;
	using cmat2x2b = cmat2x2<std::int8_t>;
	using cmat2x3b = cmat2x3<std::int8_t>;
	using cmat2x4b = cmat2x4<std::int8_t>;
	using cmat3x1b = cmat3x1<std::int8_t>;
	using cmat3x2b = cmat3x2<std::int8_t>;
	using cmat3x3b = cmat3x3<std::int8_t>;
	using cmat3x4b = cmat3x4<std::int8_t>;
	using cmat4x1b = cmat4x1<std::int8_t>;
	using cmat4x2b = cmat4x2<std::int8_t>;
	using cmat4x3b = cmat4x3<std::int8_t>;
	using cmat4x4b = cmat4x4<std::int8_t>;
	using cmat1b = cmat1x1b;
	using cmat2b = cmat2x2b;
	using cmat3b = cmat3x3b;
	using cmat4b = cmat4x4b;

	using cmat1x1s = cmat1x1<std::int16_t>;
	using cmat1x2s = cmat1x2<std::int16_t>;
	using cmat1x3s = cmat1x3<std::int16_t>;
	using cmat1x4s = cmat1x4<std::int16_t>;
	using cmat2x1s = cmat2x1<std::int16_t>;
	using cmat2x2s = cmat2x2<std::int16_t>;
	using cmat2x3s = cmat2x3<std::int16_t>;
	using cmat2x4s = cmat2x4<std::int16_t>;
	using cmat3x1s = cmat3x1<std::int16_t>;
	using cmat3x2s = cmat3x2<std::int16_t>;
	using cmat3x3s = cmat3x3<std::int16_t>;
	using cmat3x4s = cmat3x4<std::int16_t>;
	using cmat4x1s = cmat4x1<std::int16_t>;
	using cmat4x2s = cmat4x2<std::int16_t>;
	using cmat4x3s = cmat4x3<std::int16_t>;
	using cmat4x4s = cmat4x4<std::int16_t>;
	using cmat1s = cmat1x1s;
	using cmat2s = cmat2x2s;
	using cmat3s = cmat3x3s;
	using cmat4s = cmat4x4s;

	using cmat1x1i = cmat1x1<std::int32_t>;
	using cmat1x2i = cmat1x2<std::int32_t>;
	using cmat1x3i = cmat1x3<std::int32_t>;
	using cmat1x4i = cmat1x4<std::int32_t>;
	using cmat2x1i = cmat2x1<std::int32_t>;
	using cmat2x2i = cmat2x2<std::int32_t>;
	using cmat2x3i = cmat2x3<std::int32_t>;
	using cmat2x4i = cmat2x4<std::int32_t>;
	using cmat3x1i = cmat3x1<std::int32_t>;
	using cmat3x2i = cmat3x2<std::int32_t>;
	using cmat3x3i = cmat3x3<std::int32_t>;
	using cmat3x4i = cmat3x4<std::int32_t>;
	using cmat4x1i = cmat4x1<std::int32_t>;
	using cmat4x2i = cmat4x2<std::int32_t>;
	using cmat4x3i = cmat4x3<std::int32_t>;
	using cmat4x4i = cmat4x4<std::int32_t>;
	using cmat1i = cmat1x1i;
	using cmat2i = cmat2x2i;
	using cmat3i = cmat3x3i;
	using cmat4i = cmat4x4i;

	using cmat1x1l = cmat1x1<std::int64_t>;
	using cmat1x2l = cmat1x2<std::int64_t>;
	using cmat1x3l = cmat1x3<std::int64_t>;
	using cmat1x4l = cmat1x4<std::int64_t>;
	using cmat2x1l = cmat2x1<std::int64_t>;
	using cmat2x2l = cmat2x2<std::int64_t>;
	using cmat2x3l = cmat2x3<std::int64_t>;
	using cmat2x4l = cmat2x4<std::int64_t>;
	using cmat3x1l = cmat3x1<std::int64_t>;
	using cmat3x2l = cmat3x2<std::int64_t>;
	using cmat3x3l = cmat3x3<std::int64_t>;
	using cmat3x4l = cmat3x4<std::int64_t>;
	using cmat4x1l = cmat4x1<std::int64_t>;
	using cmat4x2l = cmat4x2<std::int64_t>;
	using cmat4x3l = cmat4x3<std::int64_t>;
	using cmat4x4l = cmat4x4<std::int64_t>;
	using cmat1l = cmat1x1l;
	using cmat2l = cmat2x2l;
	using cmat3l = cmat3x3l;
	using cmat4l = cmat4x4l;


	template <typename T>
	using mat1x1 = mat<T, 1, 1>;
	template <typename T>
	using mat1x2 = mat<T, 1, 2>;
	template <typename T>
	using mat1x3 = mat<T, 1, 3>;
	template <typename T>
	using mat1x4 = mat<T, 1, 4>;
	template <typename T>
	using mat2x1 = mat<T, 2, 1>;
	template <typename T>
	using mat2x2 = mat<T, 2, 2>;
	template <typename T>
	using mat2x3 = mat<T, 2, 3>;
	template <typename T>
	using mat2x4 = mat<T, 2, 4>;
	template <typename T>
	using mat3x1 = mat<T, 3, 1>;
	template <typename T>
	using mat3x2 = mat<T, 3, 2>;
	template <typename T>
	using mat3x3 = mat<T, 3, 3>;
	template <typename T>
	using mat3x4 = mat<T, 3, 4>;
	template <typename T>
	using mat4x1 = mat<T, 4, 1>;
	template <typename T>
	using mat4x2 = mat<T, 4, 2>;
	template <typename T>
	using mat4x3 = mat<T, 4, 3>;
	template <typename T>
	using mat4x4 = mat<T, 4, 4>;
	template <typename T>
	using mat1 = mat1x1<T>;
	template <typename T>
	using mat2 = mat2x2<T>;
	template <typename T>
	using mat3 = mat3x3<T>;
	template <typename T>
	using mat4 = mat4x4<T>;

	using mat1x1f = mat1x1<float>;
	using mat1x2f = mat1x2<float>;
	using mat1x3f = mat1x3<float>;
	using mat1x4f = mat1x4<float>;
	using mat2x1f = mat2x1<float>;
	using mat2x2f = mat2x2<float>;
	using mat2x3f = mat2x3<float>;
	using mat2x4f = mat2x4<float>;
	using mat3x1f = mat3x1<float>;
	using mat3x2f = mat3x2<float>;
	using mat3x3f = mat3x3<float>;
	using mat3x4f = mat3x4<float>;
	using mat4x1f = mat4x1<float>;
	using mat4x2f = mat4x2<float>;
	using mat4x3f = mat4x3<float>;
	using mat4x4f = mat4x4<float>;
	using mat1f = mat1x1f;
	using mat2f = mat2x2f;
	using mat3f = mat3x3f;
	using mat4f = mat4x4f;

	using mat1x1d = mat1x1<double>;
	using mat1x2d = mat1x2<double>;
	using mat1x3d = mat1x3<double>;
	using mat1x4d = mat1x4<double>;
	using mat2x1d = mat2x1<double>;
	using mat2x2d = mat2x2<double>;
	using mat2x3d = mat2x3<double>;
	using mat2x4d = mat2x4<double>;
	using mat3x1d = mat3x1<double>;
	using mat3x2d = mat3x2<double>;
	using mat3x3d = mat3x3<double>;
	using mat3x4d = mat3x4<double>;
	using mat4x1d = mat4x1<double>;
	using mat4x2d = mat4x2<double>;
	using mat4x3d = mat4x3<double>;
	using mat4x4d = mat4x4<double>;
	using mat1d = mat1x1d;
	using mat2d = mat2x2d;
	using mat3d = mat3x3d;
	using mat4d = mat4x4d;

	using mat1x1b = mat1x1<std::int8_t>;
	using mat1x2b = mat1x2<std::int8_t>;
	using mat1x3b = mat1x3<std::int8_t>;
	using mat1x4b = mat1x4<std::int8_t>;
	using mat2x1b = mat2x1<std::int8_t>;
	using mat2x2b = mat2x2<std::int8_t>;
	using mat2x3b = mat2x3<std::int8_t>;
	using mat2x4b = mat2x4<std::int8_t>;
	using mat3x1b = mat3x1<std::int8_t>;
	using mat3x2b = mat3x2<std::int8_t>;
	using mat3x3b = mat3x3<std::int8_t>;
	using mat3x4b = mat3x4<std::int8_t>;
	using mat4x1b = mat4x1<std::int8_t>;
	using mat4x2b = mat4x2<std::int8_t>;
	using mat4x3b = mat4x3<std::int8_t>;
	using mat4x4b = mat4x4<std::int8_t>;
	using mat1b = mat1x1b;
	using mat2b = mat2x2b;
	using mat3b = mat3x3b;
	using mat4b = mat4x4b;

	using mat1x1s = mat1x1<std::int16_t>;
	using mat1x2s = mat1x2<std::int16_t>;
	using mat1x3s = mat1x3<std::int16_t>;
	using mat1x4s = mat1x4<std::int16_t>;
	using mat2x1s = mat2x1<std::int16_t>;
	using mat2x2s = mat2x2<std::int16_t>;
	using mat2x3s = mat2x3<std::int16_t>;
	using mat2x4s = mat2x4<std::int16_t>;
	using mat3x1s = mat3x1<std::int16_t>;
	using mat3x2s = mat3x2<std::int16_t>;
	using mat3x3s = mat3x3<std::int16_t>;
	using mat3x4s = mat3x4<std::int16_t>;
	using mat4x1s = mat4x1<std::int16_t>;
	using mat4x2s = mat4x2<std::int16_t>;
	using mat4x3s = mat4x3<std::int16_t>;
	using mat4x4s = mat4x4<std::int16_t>;
	using mat1s = mat1x1s;
	using mat2s = mat2x2s;
	using mat3s = mat3x3s;
	using mat4s = mat4x4s;

	using mat1x1i = mat1x1<std::int32_t>;
	using mat1x2i = mat1x2<std::int32_t>;
	using mat1x3i = mat1x3<std::int32_t>;
	using mat1x4i = mat1x4<std::int32_t>;
	using mat2x1i = mat2x1<std::int32_t>;
	using mat2x2i = mat2x2<std::int32_t>;
	using mat2x3i = mat2x3<std::int32_t>;
	using mat2x4i = mat2x4<std::int32_t>;
	using mat3x1i = mat3x1<std::int32_t>;
	using mat3x2i = mat3x2<std::int32_t>;
	using mat3x3i = mat3x3<std::int32_t>;
	using mat3x4i = mat3x4<std::int32_t>;
	using mat4x1i = mat4x1<std::int32_t>;
	using mat4x2i = mat4x2<std::int32_t>;
	using mat4x3i = mat4x3<std::int32_t>;
	using mat4x4i = mat4x4<std::int32_t>;
	using mat1i = mat1x1i;
	using mat2i = mat2x2i;
	using mat3i = mat3x3i;
	using mat4i = mat4x4i;

	using mat1x1l = mat1x1<std::int64_t>;
	using mat1x2l = mat1x2<std::int64_t>;
	using mat1x3l = mat1x3<std::int64_t>;
	using mat1x4l = mat1x4<std::int64_t>;
	using mat2x1l = mat2x1<std::int64_t>;
	using mat2x2l = mat2x2<std::int64_t>;
	using mat2x3l = mat2x3<std::int64_t>;
	using mat2x4l = mat2x4<std::int64_t>;
	using mat3x1l = mat3x1<std::int64_t>;
	using mat3x2l = mat3x2<std::int64_t>;
	using mat3x3l = mat3x3<std::int64_t>;
	using mat3x4l = mat3x4<std::int64_t>;
	using mat4x1l = mat4x1<std::int64_t>;
	using mat4x2l = mat4x2<std::int64_t>;
	using mat4x3l = mat4x3<std::int64_t>;
	using mat4x4l = mat4x4<std::int64_t>;
	using mat1l = mat1x1l;
	using mat2l = mat2x2l;
	using mat3l = mat3x3l;
	using mat4l = mat4x4l;
#pragma endregion
}

#include "Matrix.inl"
