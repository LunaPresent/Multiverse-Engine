#include "Matrix.h"

#include <cmath> // sin, cos, tan, abs



template <typename T, unsigned int R, unsigned int C, bool D>
mv::Matrix<T, R, C, D>::Matrix()
	: _elements{}
{}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename... V>
mv::Matrix<T, R, C, D>::Matrix(const V&... elements)
	: _elements{ static_cast<T>(elements)... }
{}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename... V>
mv::Matrix<T, R, C, D>::Matrix(const mv::Vector<V, mv::Matrix<T, R, C, D>::inner_dimension, D>&... vectors)
	: _vectors{ vectors... }
{}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename... V>
mv::Matrix<T, R, C, D>::Matrix(const mv::Vector<V, mv::Matrix<T, R, C, D>::outer_dimension, !D>&... vectors)
	: Matrix(Matrix<T, R, C, !D>{ vectors... })
{}

template <typename T, unsigned int R, unsigned int C, bool D>
mv::Matrix<T, R, C, D>::Matrix(const Matrix<T, R, C, !D>& matrix)
{
	for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
		for (unsigned int j{ 0 }; j < this->inner_dimension; ++j) {
			this->at(i)[j] = matrix[j][i];
		}
	}
}



template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R>
mv::Matrix<T, R, C, D>::operator typename std::enable_if<_R == 1, typename mv::Matrix<T, R, C, D>::row_type>::type() const
{
	return this->get_row(0);
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _C>
mv::Matrix<T, R, C, D>::operator typename std::enable_if<_C == 1, typename mv::Matrix<T, R, C, D>::column_type>::type() const
{
	return this->get_column(0);
}



template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, const mv::Matrix<T, R, C, D>&>::type mv::Matrix<T, R, C, D>::identity()
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	static mv::Matrix<T, R, C, D> retval{ mv::Matrix<T, R, C, D>::_cache_init_identity() };
	return retval;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::translate(const mv::Vector<T, mv::Matrix<T, R, C, D>::column_count - 1, mv::ROW>& translation_row_vector)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	mv::Matrix<T, R, C, D> retval{ mv::Matrix<T, R, C, D>::identity() };
	for (unsigned int i{ 0 }; i < mv::Matrix<T, R, C, D>::column_count - 1; ++i) {
		retval(mv::Matrix<T, R, C, D>::row_count - 1, i) = translation_row_vector[i];
	}
	return retval;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::translate(const mv::Vector<T, mv::Matrix<T, R, C, D>::row_count - 1, mv::COLUMN>& translation_column_vector)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	mv::Matrix<T, R, C, D> retval{ mv::Matrix<T, R, C, D>::identity() };
	for (unsigned int i{ 0 }; i < mv::Matrix<T, R, C, D>::row_count - 1; ++i) {
		retval(i, mv::Matrix<T, R, C, D>::column_count - 1) = translation_column_vector[i];
	}
	return retval;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D, unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::scale(const mv::Vector<T, mv::Matrix<T, R, C, D>::outer_dimension - 1, _D>& scale_vector)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	mv::Matrix<T, R, C, D> retval{};
	for (unsigned int i{ 0 }; i < retval.outer_dimension; ++i) {
		for (unsigned int j{ 0 }; j < retval.inner_dimension; ++j) {
			if (i != j) {
				retval[i][j] = mv::Matrix<T, R, C, D>::value_type{ 0 };
			}
			else if (i != mv::Matrix<T, R, C, D>::outer_dimension - 1) {
				retval[i][i] = scale_vector[i];
			}
			else {
				retval[i][i] = mv::Matrix<T, R, C, D>::value_type{ 1 };
			}
		}
	}
	return retval;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::scale(const T& uniform_scalar)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	mv::Matrix<T, R, C, D> retval{};
	for (unsigned int i{ 0 }; i < retval.outer_dimension; ++i) {
		for (unsigned int j{ 0 }; j < retval.inner_dimension; ++j) {
			if (i != j) {
				retval[i][j] = mv::Matrix<T, R, C, D>::value_type{ 0 };
			}
			else if (i != mv::Matrix<T, R, C, D>::outer_dimension - 1) {
				retval[i][i] = uniform_scalar;
			}
			else {
				retval[i][i] = mv::Matrix<T, R, C, D>::value_type{ 1 };
			}
		}
	}
	return retval;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D, unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::rotate(double angle)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(angle) };
	double sina{ std::sin(angle) };
	if (_D == D) {
		return {
			cosa  , sina  , T{ 0 },
			-sina , cosa  , T{ 0 },
			T{ 0 }, T{ 0 }, T{ 1 }
		};
	}
	else {
		return {
			cosa  , -sina , T{ 0 },
			sina  , cosa  , T{ 0 },
			T{ 0 }, T{ 0 }, T{ 1 }
		};
	}
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::rotate(
	double angle,
	const mv::Vector<T, 3, D>& r)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(angle) };
	double sina{ std::sin(angle) };
	return {
		cosa + r.x * r.x * (1 - cosa)      , r.y * r.x * (1 - cosa) + r.z * sina, r.z * r.x * (1 - cosa) - r.y * sina, T{ 0 },
		r.x * r.y * (1 - cosa) - r.z * sina, cosa + r.y * r.y * (1 - cosa)      , r.z * r.y * (1 - cosa) + r.x * sina, T{ 0 },
		r.x * r.z * (1 - cosa) + r.y * sina, r.y * r.z * (1 - cosa) - r.x * sina, cosa + r.z * r.z * (1 - cosa)      , T{ 0 },
		T{ 0 }                             , T{ 0 }                             , T{ 0 }                             , T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::rotate(
	double angle,
	const mv::Vector<T, 3, !D>& r)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(angle) };
	double sina{ std::sin(angle) };
	return {
		cosa + r.x * r.x * (1 - cosa)      , r.x * r.y * (1 - cosa) - r.z * sina, r.x * r.z * (1 - cosa) + r.y * sina, T{ 0 },
		r.y * r.x * (1 - cosa) + r.z * sina, cosa + r.y * r.y * (1 - cosa)      , r.y * r.z * (1 - cosa) - r.x * sina, T{ 0 },
		r.z * r.x * (1 - cosa) - r.y * sina, r.z * r.y * (1 - cosa) + r.x * sina, cosa + r.z * r.z * (1 - cosa)      , T{ 0 },
		T{ 0 }                             , T{ 0 }                             , T{ 0 }                             , T{ 1 }
	};
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(
	const mv::Vector<T, 2, D>& t,
	double a,
	const mv::Vector<T, 2, D>& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(a) };
	double sina{ std::sin(a) };
	return {
		s.x * cosa , s.x * sina, T{ 0 },
		s.y * -sina, s.y * cosa, T{ 0 },
		t.x        , t.y       , T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(
	const mv::Vector<T, 2, D>& t,
	double a,
	const T& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(a) };
	double sina{ std::sin(a) };
	return {
		s * cosa , s * sina, T{ 0 },
		s * -sina, s * cosa, T{ 0 },
		t.x      , t.y     , T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(
	const mv::Vector<T, 2, !D>& t,
	double a,
	const mv::Vector<T, 2, !D>& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(a) };
	double sina{ std::sin(a) };
	return {
		s.x * cosa, s.y * -sina, t.x,
		s.x * sina, s.y * cosa , t.y,
		T{ 0 }    , T{ 0 }     , T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(
	const mv::Vector<T, 2, !D>& t,
	double a,
	const T& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	double cosa{ std::cos(a) };
	double sina{ std::sin(a) };
	return {
		s * cosa, s * -sina, t.x,
		s * sina, s * cosa , t.y,
		T{ 0 }  , T{ 0 }   , T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(const Vector<T, 3, D>& t, const Quaternion<T>& r, const Vector<T, 3, D>& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	return {
		s.x * (T{ 1 } - 2 * (r.y * r.y - r.z * r.z)), s.x * 2 * (r.x * r.y + r.z * r.w), s.x * 2 * (r.x * r.z - r.y * r.w), T{ 0 },
		s.y * 2 * (r.x * r.y - r.z * r.w), s.y * (T{ 1 } - 2 * (r.x * r.x - r.z * r.z)), s.y * 2 * (r.y * r.z + r.x * r.w), T{ 0 },
		s.z * 2 * (r.x * r.z + r.y * r.w), s.z * 2 * (r.y * r.z - r.x * r.w), s.z * (T{ 1 } - 2 * (r.x * r.x - r.y * r.y)), T{ 0 },
		t.x, t.y, t.z, T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(const Vector<T, 3, D>& t, const Quaternion<T>& r, const T& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	return {
		s * (T{ 1 } - 2 * (r.y * r.y - r.z * r.z)), s * 2 * (r.x * r.y + r.z * r.w), s * 2 * (r.x * r.z - r.y * r.w), T{ 0 },
		s * 2 * (r.x * r.y - r.z * r.w), s * (T{ 1 } - 2 * (r.x * r.x - r.z * r.z)), s * 2 * (r.y * r.z + r.x * r.w), T{ 0 },
		s * 2 * (r.x * r.z + r.y * r.w), s * 2 * (r.y * r.z - r.x * r.w), s * (T{ 1 } - 2 * (r.x * r.x - r.y * r.y)), T{ 0 },
		t.x, t.y, t.z, T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(const Vector<T, 3, !D>& t, const Quaternion<T>& r, const Vector<T, 3, !D>& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	return {
		s.x * (T{ 1 } - 2 * (r.y * r.y - r.z * r.z)), s.y * 2 * (r.x * r.y - r.z * r.w), s.z * 2 * (r.x * r.z + r.y * r.w), t.x,
		s.x * 2 * (r.x * r.y + r.z * r.w), s.y * (T{ 1 } - 2 * (r.x * r.x - r.z * r.z)), s.z * 2 * (r.y * r.z - r.x * r.w), t.y,
		s.x * 2 * (r.x * r.z - r.y * r.w), s.y * 2 * (r.y * r.z + r.x * r.w), s.z * (T{ 1 } - 2 * (r.x * r.x - r.y * r.y)), t.z,
		T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }
	};
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::transform(const Vector<T, 3, !D>& t, const Quaternion<T>& r, const T& s)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	return {
		s * (T{ 1 } - 2 * (r.y * r.y - r.z * r.z)), s * 2 * (r.x * r.y - r.z * r.w), s * 2 * (r.x * r.z + r.y * r.w), t.x,
		s * 2 * (r.x * r.y + r.z * r.w), s * (T{ 1 } - 2 * (r.x * r.x - r.z * r.z)), s * 2 * (r.y * r.z - r.x * r.w), t.y,
		s * 2 * (r.x * r.z - r.y * r.w), s * 2 * (r.y * r.z + r.x * r.w), s * (T{ 1 } - 2 * (r.x * r.x - r.y * r.y)), t.z,
		T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }
	};
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::viewpoint(
	const mv::Vector<T, 2, D>& p,
	const mv::Vector<T, 2, D>& u)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 3 && _C == 3, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::viewpoint(
	const mv::Vector<T, 2, !D>& p,
	const mv::Vector<T, 2, !D>& u)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D, unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::viewpoint(
	const mv::Vector<T, 3, _D>& position_vector,
	const mv::Vector<T, 3, _D>& direction_unit_vector,
	const mv::Vector<T, 3, _D>& up_axis_vector)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	mv::Vector<T, 3, _D> right{ up_axis_vector.cross(direction_unit_vector).normalise() };
	return mv::Matrix<T, R, C, D>::viewpoint(position_vector, direction_unit_vector, direction_unit_vector.cross(right), right);
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D, unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::viewpoint(
	const mv::Vector<T, 3, _D>& p,
	const mv::Vector<T, 3, _D>& d,
	const mv::Vector<T, 3, _D>& u,
	const mv::Vector<T, 3, _D>& r)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	if (_D == D) {
		return {
			r.x      , u.x      , d.x      , T{ 0 },
			r.y      , u.y      , d.y      , T{ 0 },
			r.z      , u.z      , d.z      , T{ 0 },
			-r.dot(p), -u.dot(p), -d.dot(p), T{ 1 }
		};
	}
	else {
		return {
			r.x   , r.y   , r.z   , -r.dot(p),
			u.x   , u.y   , u.z   , -u.dot(p),
			d.x   , d.y   , d.z   , -d.dot(p),
			T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 }
		};
	}
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D, unsigned int _R, unsigned int _C>
typename std::enable_if<_R == 4 && _C == 4, mv::Matrix<T, 4, 4>>::type mv::Matrix<T, R, C, D>::perspective(
	typename mv::Matrix<T, R, C, D>::value_type view_angle,
	typename mv::Matrix<T, R, C, D>::value_type aspect_ratio,
	typename mv::Matrix<T, R, C, D>::value_type near_plane,
	typename mv::Matrix<T, R, C, D>::value_type far_plane)
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	typename mv::Matrix<T, R, C, D>::value_type tan_a{ std::tan(view_angle / 2) };

	if (_D == D) {
		return {
			1 / (tan_a * aspect_ratio), T{ 0 }   , T{ 0 }                                               , T{ 0 } ,
			T{ 0 }                    , 1 / tan_a, T{ 0 }                                               , T{ 0 } ,
			T{ 0 }                    , T{ 0 }   , (near_plane + far_plane) / (near_plane - far_plane)  , T{ -1 },
			T{ 0 }                    , T{ 0 }   , 2 * near_plane * far_plane / (near_plane - far_plane), T{ 0 }
		};
	}
	else {
		return {
			1 / (tan_a * aspect_ratio), T{ 0 }   , T{ 0 }                                             , T{ 0 }                                               ,
			T{ 0 }                    , 1 / tan_a, T{ 0 }                                             , T{ 0 }                                               ,
			T{ 0 }                    , T{ 0 }   , (near_plane + far_plane) / (near_plane - far_plane), 2 * near_plane * far_plane / (near_plane - far_plane),
			T{ 0 }                    , T{ 0 }   , T{ -1 }                                            , T{ 0 }
		};
	}
}



template <typename T, unsigned int R, unsigned int C, bool D>
typename mv::Matrix<T, R, C, D>::vector_type& mv::Matrix<T, R, C, D>::operator[](unsigned int i)
{
	return this->_vectors[i];
}

template <typename T, unsigned int R, unsigned int C, bool D>
const typename mv::Matrix<T, R, C, D>::vector_type& mv::Matrix<T, R, C, D>::operator[](unsigned int i) const
{
	return this->_vectors[i];
}

template <typename T, unsigned int R, unsigned int C, bool D>
typename mv::Matrix<T, R, C, D>::vector_type& mv::Matrix<T, R, C, D>::at(unsigned int i)
{
	return this->_vectors[i];
}

template <typename T, unsigned int R, unsigned int C, bool D>
const typename mv::Matrix<T, R, C, D>::vector_type& mv::Matrix<T, R, C, D>::at(unsigned int i) const
{
	return this->_vectors[i];
}

template <typename T, unsigned int R, unsigned int C, bool D>
typename mv::Matrix<T, R, C, D>::value_type& mv::Matrix<T, R, C, D>::operator()(unsigned int row, unsigned int column)
{
	return (this->data_order == mv::ROW ? this->_vectors[row][column] : this->_vectors[column][row]);
}

template <typename T, unsigned int R, unsigned int C, bool D>
const typename mv::Matrix<T, R, C, D>::value_type& mv::Matrix<T, R, C, D>::operator()(unsigned int row, unsigned int column) const
{
	return (this->data_order == mv::ROW ? this->_vectors[row][column] : this->_vectors[column][row]);
}

template <typename T, unsigned int R, unsigned int C, bool D>
typename mv::Matrix<T, R, C, D>::value_type& mv::Matrix<T, R, C, D>::at(unsigned int row, unsigned int column)
{
	return (this->data_order == mv::ROW ? this->_vectors[row][column] : this->_vectors[column][row]);
}

template <typename T, unsigned int R, unsigned int C, bool D>
const typename mv::Matrix<T, R, C, D>::value_type& mv::Matrix<T, R, C, D>::at(unsigned int row, unsigned int column) const
{
	return (this->data_order == mv::ROW ? this->_vectors[row][column] : this->_vectors[column][row]);
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::ROW, const typename mv::Matrix<T, R, C, D>::row_type&>::type mv::Matrix<T, R, C, D>::get_row(unsigned int row_index) const
{
	return this->at(row_index);
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::COLUMN, typename mv::Matrix<T, R, C, D>::row_type>::type mv::Matrix<T, R, C, D>::get_row(unsigned int row_index) const
{
	typename mv::Matrix<T, R, C, D>::row_type row_vector;
	for (unsigned int i{ 0 }; i < row_vector.dimension; ++i) {
		row_vector[i] = this->at(i)[row_index];
	}
	return row_vector;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::COLUMN, const typename mv::Matrix<T, R, C, D>::column_type&>::type mv::Matrix<T, R, C, D>::get_column(unsigned int column_index) const
{
	return this->at(column_index);
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::ROW, typename mv::Matrix<T, R, C, D>::column_type>::type mv::Matrix<T, R, C, D>::get_column(unsigned int column_index) const
{
	typename mv::Matrix<T, R, C, D>::column_type column_vector;
	for (unsigned int i{ 0 }; i < column_vector.dimension; ++i) {
		column_vector[i] = this->at(i)[column_index];
	}
	return column_vector;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::ROW, void>::type mv::Matrix<T, R, C, D>::set_row(unsigned int row_index, const typename mv::Matrix<T, R, C, D>::row_type& row)
{
	this->at(row_index) = row;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::COLUMN, void>::type mv::Matrix<T, R, C, D>::set_row(unsigned int row_index, const typename mv::Matrix<T, R, C, D>::row_type& row)
{
	for (unsigned int i{ 0 }; i < this->column_count; ++i) {
		this->at(i)[row_index] = row[i];
	}
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::COLUMN, void>::type mv::Matrix<T, R, C, D>::set_column(unsigned int column_index, const typename mv::Matrix<T, R, C, D>::column_type& column)
{
	this->at(column_index) = column;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <bool _D>
typename std::enable_if<_D == mv::ROW, void>::type mv::Matrix<T, R, C, D>::set_column(unsigned int column_index, const typename mv::Matrix<T, R, C, D>::column_type& column)
{
	for (unsigned int i{ 0 }; i < this->row_count; ++i) {
		this->at(i)[column_index] = column[i];
	}
}



template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, typename mv::Matrix<T, R, C, D>::value_type>::type mv::Matrix<T, R, C, D>::determinant() const
{
	unsigned int permutation[this->outer_dimension];
	for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
		permutation[i] = i;
	}
	bool invert{ false };

	auto determinant_step = [&]() {
		typename mv::Matrix<T, R, C, D>::value_type step_value{
			invert ? typename mv::Matrix<T, R, C, D>::value_type{ -1 } : typename mv::Matrix<T, R, C, D>::value_type{ 1 }
		};
		for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
			step_value *= this->at(i)[permutation[i]];
		}
		invert = !invert;
		return step_value;
	};

	// initialise at product of original permutation
	typename mv::Matrix<T, R, C, D>::value_type retval{ determinant_step() };

	unsigned int c[this->outer_dimension]{};
	for (unsigned int i{ 0 }; i < this->outer_dimension;) {
		if (c[i] < i) {
			unsigned int swap{ permutation[i] };
			if (i % 2 == 0) {
				permutation[i] = permutation[0];
				permutation[0] = swap;
			}
			else {
				permutation[i] = permutation[c[i]];
				permutation[c[i]] = swap;
			}

			retval += determinant_step();

			++c[i];
			i = 0;
		}
		else {
			c[i] = 0;
			++i;
		}
	}

	return retval;
}


template <typename T, unsigned int R, unsigned int C, bool D>
const mv::Matrix<T, C, R, static_cast<bool>(!static_cast<bool>(D))>& mv::Matrix<T, R, C, D>::transpose() const
{
	return *reinterpret_cast<const mv::Matrix<T, C, R, static_cast<bool>(!static_cast<bool>(D))>*>(this);
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::inverse() const
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	// Gauss-Jordan algorithm
	mv::Matrix<T, R, C, D> mat{ *this };
	mv::Matrix<T, R, C, D> inv{ this->identity() };
	for (unsigned int p{ 0 }; p < this->outer_dimension; ++p) {
		// get highest possible value to set pivot to, to avoid getting a value just slightly off zero
		unsigned int i_max{ p };
		for (unsigned int i{ p + 1 }; i < this->outer_dimension; ++i) {
			if (std::abs(mat[i][p]) > std::abs(mat[i_max][p])) {
				i_max = i;
			}
		}
		// swap row i_max and pivot row
		typename mv::Matrix<T, R, C, D>::vector_type mat_swap{ mat[i_max] };
		mat[i_max] = mat[p];
		mat[p] = mat_swap;
		// and repeat operation on the inverse matrix
		typename mv::Matrix<T, R, C, D>::vector_type inv_swap{ inv[i_max] };
		inv[i_max] = inv[p];
		inv[p] = inv_swap;

		for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
			if (i == p) {
				continue;
			}
			T f{ mat[i][p] / mat[p][p] };
			for (unsigned int j{ 0 }; j < this->inner_dimension; ++j) {
				mat[i][j] -= mat[p][j] * f;
				inv[i][j] -= inv[p][j] * f;
			}
		}
	}
	for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
		for (unsigned int j{ 0 }; j < this->inner_dimension; ++j) {
			inv[i][j] /= mat[i][i];
		}
	}
	return inv;
}



template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T, bool _D>
bool mv::Matrix<T, R, C, D>::operator==(const mv::Matrix<_T, R, C, _D>& rhs) const
{
	for (unsigned int row{ 0 }; row < this->row_count; ++row) {
		for (unsigned int column{ 0 }; column < this->column_count; ++column) {
			if (this->at(row, column) != rhs(row, column)) {
				return false;
			}
		}
	}
	return true;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T, bool _D>
bool mv::Matrix<T, R, C, D>::operator!=(const mv::Matrix<_T, R, C, _D>& rhs) const
{
	return !(*this == rhs);
}



template <typename T, unsigned int R, unsigned int C, bool D>
mv::Matrix<T, R, C, D> mv::Matrix<T, R, C, D>::operator-() const
{
	Matrix<T, R, C, D> retval{};
	for (unsigned int row{ 0 }; row < this->row_count; ++row) {
		for (unsigned int column{ 0 }; column < this->column_count; ++column) {
			retval(row, column) = -(this->at(row, column));
		}
	}
	return retval;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T, bool _D>
mv::Matrix<T, R, C, D>& mv::Matrix<T, R, C, D>::operator+=(const mv::Matrix<_T, R, C, _D>& rhs)
{
	for (unsigned int row{ 0 }; row < this->row_count; ++row) {
		for (unsigned int column{ 0 }; column < this->column_count; ++column) {
			this->at(row, column) += rhs(row, column);
		}
	}
	return *this;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T, bool _D>
mv::Matrix<T, R, C, D> mv::Matrix<T, R, C, D>::operator+(const mv::Matrix<_T, R, C, _D>& rhs) const
{
	mv::Matrix<T, R, C, D> retval{ *this };
	return retval += rhs;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T, bool _D>
mv::Matrix<T, R, C, D>& mv::Matrix<T, R, C, D>::operator-=(const mv::Matrix<_T, R, C, _D>& rhs)
{
	for (unsigned int row{ 0 }; row < this->row_count; ++row) {
		for (unsigned int column{ 0 }; column < this->column_count; ++column) {
			this->at(row, column) -= rhs(row, column);
		}
	}
	return *this;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T, bool _D>
mv::Matrix<T, R, C, D> mv::Matrix<T, R, C, D>::operator-(const mv::Matrix<_T, R, C, _D>& rhs) const
{
	mv::Matrix<T, R, C, D> retval{ *this };
	return retval -= rhs;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T>
mv::Matrix<T, R, C, D>& mv::Matrix<T, R, C, D>::operator*=(const _T& rhs)
{
	for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
		this->at(i) *= rhs;
	}
	return *this;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T>
mv::Matrix<T, R, C, D> mv::Matrix<T, R, C, D>::operator*(const _T& rhs) const
{
	mv::Matrix<T, R, C, D> retval{ *this };
	return retval *= rhs;
}


template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T>
mv::Matrix<T, R, C, D>& mv::Matrix<T, R, C, D>::operator/=(const _T& rhs)
{
	for (unsigned int i{ 0 }; i < this->outer_dimension; ++i) {
		this->at(i) /= rhs;
	}
	return *this;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename _T>
mv::Matrix<T, R, C, D> mv::Matrix<T, R, C, D>::operator/(const _T& rhs) const
{
	mv::Matrix<T, R, C, D> retval{ *this };
	return retval /= rhs;
}


template<typename T, unsigned int R, unsigned int C, bool D>
template<typename TR, bool DR>
mv::Matrix<T, R, C, D>& mv::Matrix<T, R, C, D>::operator*=(const mv::Matrix<TR, C, C, DR>& rhs)
{
	mv::Matrix<T, R, C, D> retval{};
	for (unsigned int r{ 0 }; r < retval.row_count; ++r) {
		for (unsigned int c{ 0 }; c < retval.column_count; ++c) {
			for (unsigned int i{ 0 }; i < this->column_count; ++i) {
				retval(r, c) += this->at(r, i) * rhs(i, c);
			}
		}
	}
	return *this = retval;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename TR, unsigned int CR, bool DR>
mv::Matrix<decltype(std::declval<T>() * std::declval<TR>()), R, CR, D> mv::Matrix<T, R, C, D>::operator*(const mv::Matrix<TR, C, CR, DR>& rhs) const
{
	mv::Matrix<decltype(std::declval<T>() * std::declval<TR>()), R, CR, D> retval{};
	for (unsigned int r{ 0 }; r < retval.row_count; ++r) {
		for (unsigned int c{ 0 }; c < retval.column_count; ++c) {
			for (unsigned int i{ 0 }; i < this->column_count; ++i) {
				retval(r, c) += this->at(r, i) * rhs(i, c);
			}
		}
	}
	return retval;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename TR>
mv::Vector<decltype(std::declval<T>() * std::declval<TR>()), R, mv::COLUMN> mv::Matrix<T, R, C, D>::operator*(const mv::Vector<TR, C, mv::COLUMN>& rhs) const
{
	mv::Vector<decltype(std::declval<T>() * std::declval<TR>()), R, COLUMN> retval{};
	for (unsigned int r{ 0 }; r < retval.dimension; ++r) {
		for (unsigned int i{ 0 }; i < this->column_count; ++i) {
			retval[r] += this->at(r, i) * rhs[i];
		}
	}
	return retval;
}

template <typename T, unsigned int R, unsigned int C, bool D>
template <typename TR, unsigned int CR, unsigned int _C>
typename std::enable_if<_C == 1, mv::Matrix<decltype(std::declval<T>() * std::declval<TR>()), R, CR, D>>::type mv::Matrix<T, R, C, D>::operator*(const mv::Vector<TR, CR, ROW>& rhs)
{
	mv::Matrix<decltype(std::declval<T>() * std::declval<TR>()), R, CR, D> retval{};
	for (unsigned int r{ 0 }; r < retval.row_count; ++r) {
		for (unsigned int c{ 0 }; c < retval.column_count; ++c) {
			retval(r, c) = this->at(r, 0) * rhs[c];
		}
	}
	return retval;
}



template<typename T, unsigned int R, unsigned int C, bool D>
template<unsigned int _R, unsigned int _C>
typename std::enable_if<_R == _C, mv::Matrix<T, R, C, D>>::type mv::Matrix<T, R, C, D>::_cache_init_identity()
{
	static_assert(_R == R && _C == C, "template arguments do not match default");

	mv::Matrix<T, R, C, D> retval{};
	for (unsigned int i{ 0 }; i < retval.outer_dimension; ++i) {
		for (unsigned int j{ 0 }; j < retval.inner_dimension; ++j) {
			retval[i][j] = (i == j ? mv::Matrix<T, R, C, D>::value_type{ 1 } : mv::Matrix<T, R, C, D>::value_type{ 0 });
		}
	}
	return retval;
}




template <typename _T, typename T, unsigned int R, unsigned int C, bool D>
mv::Matrix<T, R, C, D> mv::operator*(const _T& lhs, const mv::Matrix<T, R, C, D>& rhs) {
	return rhs * lhs;
}


template <typename TL, typename TR, unsigned int R, unsigned int C>
mv::Matrix<decltype(std::declval<TL>() * std::declval<TR>()), R, C> operator*(const mv::Vector<TL, R, mv::COLUMN>& lhs, const mv::Vector<TR, C, mv::ROW>& rhs)
{
	mv::Matrix<decltype(std::declval<TL>() * std::declval<TR>()), R, C> retval{};
	for (unsigned int r{ 0 }; r < retval.row_count; ++r) {
		for (unsigned int c{ 0 }; c < retval.column_count; ++c) {
			retval(r, c) = lhs[r] * rhs[c];
		}
	}
	return retval;
}


template<typename TL, typename TR, unsigned int C, bool D>
mv::Vector<TL, C, mv::ROW>& mv::operator*=(mv::Vector<TL, C, mv::ROW>& lhs, const mv::Matrix<TR, C, C, D>& rhs)
{
	mv::Vector<TL, C, mv::ROW> retval{};
	for (unsigned int c{ 0 }; c < retval.dimension; ++c) {
		for (unsigned int i{ 0 }; i < lhs.dimension; ++i) {
			retval[c] += lhs[i] * rhs(i, c);
		}
	}
	return lhs = retval;
}

template <typename TL, typename TR, unsigned int C, unsigned int CR, bool D>
mv::Vector<decltype(std::declval<TL>() * std::declval<TR>()), CR, mv::ROW> mv::operator*(const mv::Vector<TL, C, mv::ROW>& lhs, const mv::Matrix<TR, C, CR, D>& rhs)
{
	mv::Vector<decltype(std::declval<TL>() * std::declval<TR>()), CR, mv::ROW> retval{};
	for (unsigned int c{ 0 }; c < retval.dimension; ++c) {
		for (unsigned int i{ 0 }; i < lhs.dimension; ++i) {
			retval[c] += lhs[i] * rhs(i, c);
		}
	}
	return retval;
}

template <typename TL, typename TR, unsigned int R, unsigned int C, bool D>
mv::Matrix<decltype(std::declval<TL>() * std::declval<TR>()), R, C, D> mv::operator*(const mv::Vector<TL, R, mv::COLUMN>& lhs, const mv::Matrix<TR, 1, C, D>& rhs)
{
	mv::Matrix<decltype(std::declval<TL>() * std::declval<TR>()), R, C, D> retval{};
	for (unsigned int r{ 0 }; r < retval.row_count; ++r) {
		for (unsigned int c{ 0 }; c < retval.column_count; ++c) {
			retval(r, c) = lhs[r] * rhs(0, c);
		}
	}
	return retval;
}



template <typename T, unsigned int R, unsigned int C, bool D>
std::ostream& mv::operator<<(std::ostream& stream, const mv::Matrix<T, R, C, D>& matrix)
{
	for (unsigned int r{ 0 }; r < matrix.row_count; ++r) {
		stream << "|";
		for (unsigned int c{ 0 }; c < matrix.column_count; ++c) {
			stream << matrix(r, c);
			if (c < matrix.column_count - 1) {
				stream << ", ";
			}
		}
		stream << "|";
		if (r < matrix.row_count) {
			stream << "\n";
		}
	}
	return stream;
}
