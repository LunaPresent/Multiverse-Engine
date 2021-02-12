#include "Vector.h"

#include <cmath> // sqrt, acos



template <typename T, unsigned int N>
template <typename... V>
inline mv::detail::VectorBase<T, N>::VectorBase(V&&... components)
	: components{ std::forward<V>(components)... }
{}


template <typename T>
inline mv::detail::VectorBase<T, 1>::VectorBase(T x)
	: components{ x }
{}


template <typename T>
inline mv::detail::VectorBase<T, 2>::VectorBase(T x, T y)
	: components{ x, y }
{}


template <typename T>
inline mv::detail::VectorBase<T, 3>::VectorBase(T x, T y, T z)
	: components{ x, y, z }
{}



template <typename T, unsigned int N, bool D>
mv::Vector<T, N, D>::Vector()
	: detail::VectorBase<T, N>()
{}

template <typename T, unsigned int N, bool D>
template <typename... V>
mv::Vector<T, N, D>::Vector(V&&... components)
	: detail::VectorBase<T, N>(std::forward<V>(components)...)
{}

template <typename T, unsigned int N, bool D>
template <unsigned int _N, typename... V>
mv::Vector<T, N, D>::Vector(const Vector<T, _N, D>& vector, V&&... components)
	: mv::Vector<T, N, D>{ vector, mv::Vector<T, N - _N, D>{ std::forward<V>(components)... } }
{}

template <typename T, unsigned int N, bool D>
template <unsigned int _N>
mv::Vector<T, N, D>::Vector(const Vector<T, _N, D>& vec0, const Vector<T, N - _N, D>& vec1)
{
	for (unsigned int i{ 0 }; i < vec0.dimension; ++i) {
		this->components[i] = vec0[i];
	}
	for (unsigned int i{ _N }; i < this->dimension; ++i) {
		this->components[i] = vec1[i - vec0.dimension];
	}
}

template <typename T, unsigned int N, bool D>
template <unsigned int _N>
mv::Vector<T, N, D>::Vector(const Vector<T, _N>& vec)
{
	for (unsigned int i{ 0 }; i < N; ++i) {
		this->components[i] = (i < _N ? vec[i] : T{ 0 });
	}
}

template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D>::Vector(const Vector<_T, N>& vec)
{
	for (unsigned int i{ 0 }; i < N; ++i) {
		this->components[i] = static_cast<T>(vec[i]);
	}
}

template <typename T, unsigned int N, bool D>
template <typename _T, unsigned int _N>
mv::Vector<T, N, D>::Vector(const Vector<_T, _N>& vec)
{
	for (unsigned int i{ 0 }; i < N; ++i) {
		this->components[i] = (i < _N ? static_cast<T>(vec[i]) : T{ 0 });
	}
}



template <typename T, unsigned int N, bool D>
const mv::Vector<T, N, D>* mv::Vector<T, N, D>::basis()
{
	static const mv::Vector<T, N, D>* retval{ mv::Vector<T, N, D>::_cache_init_basis() };
	return retval;
}



template <typename T, unsigned int N, bool D>
typename mv::Vector<T, N, D>::value_type& mv::Vector<T, N, D>::operator[](unsigned int i)
{
	return this->components[i];
}

template <typename T, unsigned int N, bool D>
const typename mv::Vector<T, N, D>::value_type& mv::Vector<T, N, D>::operator[](unsigned int i) const
{
	return this->components[i];
}

template<typename T, unsigned int N, bool D>
typename mv::Vector<T, N, D>::value_type& mv::Vector<T, N, D>::at(unsigned int i)
{
	return this->components[i];
}

template<typename T, unsigned int N, bool D>
const typename mv::Vector<T, N, D>::value_type& mv::Vector<T, N, D>::at(unsigned int i) const
{
	return this->components[i];
}



template <typename T, unsigned int N, bool D>
const mv::Vector<T, N, !D>& mv::Vector<T, N, D>::transpose() const
{
	return *reinterpret_cast<const mv::Vector<T, N, !D>*>(this);
}

template <typename T, unsigned int N, bool D>
mv::Vector<T, N, D> mv::Vector<T, N, D>::normalise() const
{
	mv::Vector<T, N, D> retval{ *this };
	const double magnitude = retval.magnitude();
	if (magnitude != 0.) {
		retval /= static_cast<T>(magnitude);
	}
	return retval;
}


template <typename T, unsigned int N, bool D>
double mv::Vector<T, N, D>::angle(const Vector<T, N, D>& reference) const
{
	return std::acos(this->dot(reference) / (this->magnitude() * reference.magnitude()));
}

template <typename T, unsigned int N, bool D>
double mv::Vector<T, N, D>::magnitude() const
{
	return std::sqrt(this->squared_magnitude());
}

template <typename T, unsigned int N, bool D>
typename mv::Vector<T, N, D>::value_type  mv::Vector<T, N, D>::squared_magnitude() const
{
	typename mv::Vector<T, N, D>::value_type sum{ 0 };
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		sum += this->at(i) * this->at(i);
	}
	return sum;
}



template <typename T, unsigned int N, bool D>
typename mv::Vector<T, N, D>::value_type mv::Vector<T, N, D>::dot(const Vector<T, N, D>& rhs) const
{
	typename mv::Vector<T, N, D>::value_type sum{ 0 };
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		sum += this->at(i) * rhs[i];
	}
	return sum;
}

template <typename T, unsigned int N, bool D>
template <typename... V>
typename std::enable_if<sizeof...(V) == N - 2, mv::Vector<T, N, D>>::type mv::Vector<T, N, D>::cross(const mv::Vector<V, N, D>&... ref_vecs) const
{
	mv::Vector<T, N, D> vectors[this->dimension - 1]{ *this, ref_vecs... };

	unsigned int permutation[this->dimension - 1];
	bool invert;

	auto determinant_step = [&](unsigned int component_index) {
		typename mv::Vector<T, N, D>::value_type step_value{ invert ? typename mv::Vector<T, N, D>::value_type{ -1 } : typename mv::Vector<T, N, D>::value_type{ 1 } };
		for (unsigned int i{ 0 }; i < this->dimension - 1; ++i) {
			step_value *= vectors[i][permutation[i] + (permutation[i] >= component_index ? 1 : 0)];
		}
		invert = !invert;
		return step_value;
	};

	unsigned int c[this->dimension - 1];

	mv::Vector<T, N, D> retval{};
	for (unsigned int component_index{ 0 }; component_index < retval.dimension; ++component_index) {
		for (unsigned int i{ 0 }; i < this->dimension - 1; ++i) {
			permutation[i] = i;
			c[i] = 0;
		}

		invert = component_index % 2 == this->dimension % 2;

		// initialise at product of original permutation
		typename mv::Vector<T, N, D>::value_type determinant{ determinant_step(component_index) };

		for (unsigned int i{ 0 }; i < this->dimension - 1;) {
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

				determinant += determinant_step(component_index);

				++c[i];
				i = 0;
			}
			else {
				c[i] = 0;
				++i;
			}
		}

		retval[component_index] = determinant;
	}

	return retval;
}



template<typename T, unsigned int N, bool D>
bool mv::Vector<T, N, D>::operator==(const Vector<T, N, D> rhs) const
{
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		if (this->at(i) != rhs[i]) {
			return false;
		}
	}
	return true;
}

template<typename T, unsigned int N, bool D>
bool mv::Vector<T, N, D>::operator!=(const Vector<T, N, D> rhs) const
{
	return !(*this == rhs);
}



template<typename T, unsigned int N, bool D>
mv::Vector<T, N, D> mv::Vector<T, N, D>::operator-() const
{
	mv::Vector<T, N, D> retval{};
	for (unsigned int i{ 0 }; i < retval.dimension; ++i) {
		retval[i] = -this->at(i);
	}
	return retval;
}



template <typename T, unsigned int N, bool D>
const mv::Vector<T, N, D>* mv::Vector<T, N, D>::_cache_init_basis()
{
	static mv::Vector<T, N, D> retval[mv::Vector<T, N, D>::dimension];

	for (unsigned int i{ 0 }; i < mv::Vector<T, N, D>::dimension; ++i) {
		retval[i][i] = mv::Vector<T, N, D>::value_type{ 1 };
	}
	return retval;
}



template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D>& mv::Vector<T, N, D>::operator+=(const mv::Vector<_T, N, D>& rhs)
{
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		this->at(i) += rhs[i];
	}
	return *this;
}

template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D> mv::Vector<T, N, D>::operator+(const mv::Vector<_T, N, D>& rhs) const
{
	mv::Vector<T, N, D> retval{ *this };
	return retval += rhs;
}


template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D>& mv::Vector<T, N, D>::operator-=(const mv::Vector<_T, N, D>& rhs)
{
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		this->at(i) -= rhs[i];
	}
	return *this;
}

template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D> mv::Vector<T, N, D>::operator-(const mv::Vector<_T, N, D>& rhs) const
{
	mv::Vector<T, N, D> retval{ *this };
	return retval -= rhs;
}


template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D>& mv::Vector<T, N, D>::operator*=(const _T& rhs)
{
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		this->at(i) *= rhs;
	}
	return *this;
}

template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D> mv::Vector<T, N, D>::operator*(const _T& rhs) const
{
	mv::Vector<T, N, D> retval{ *this };
	return retval *= rhs;
}


template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D>& mv::Vector<T, N, D>::operator/=(const _T& rhs)
{
	for (unsigned int i{ 0 }; i < this->dimension; ++i) {
		this->at(i) /= rhs;
	}
	return *this;
}

template <typename T, unsigned int N, bool D>
template <typename _T>
mv::Vector<T, N, D> mv::Vector<T, N, D>::operator/(const _T& rhs) const
{
	mv::Vector<T, N, D> retval{ *this };
	return retval /= rhs;
}



template <typename _T, typename T, unsigned int N, bool D>
mv::Vector<T, N, D> mv::operator*(const _T& lhs, const mv::Vector<T, N, D>& rhs) {
	return rhs * lhs;
}


template<typename TL, typename TR, unsigned int N>
decltype(std::declval<TL>() * std::declval<TR>()) mv::operator*(const mv::Vector<TL, N, ROW>& lhs, const mv::Vector<TR, N, COLUMN>& rhs)
{
	decltype(std::declval<TL>() * std::declval<TR>()) sum{ 0 };
	for (unsigned int i{ 0 }; i < lhs.dimension; ++i) {
		sum += lhs[i] * rhs[i];
	}
	return sum;
}


template <typename T, unsigned int N, bool D>
std::ostream& mv::operator<<(std::ostream& stream, const mv::Vector<T, N, D>& vec)
{
	stream << "[";
	for (unsigned int i = 0; i < vec.dimension; ++i) {
		stream << vec[i];
		if (i < vec.dimension - 1) {
			stream << ", ";
		}
	}
	stream << "]";
	return stream;
}
