#include "ServiceProxy.h"


template <typename T>
inline mv::ServiceProxy<T>::ServiceProxy(T* const* service_location)
	: _service_location{ service_location }
{}

template <typename T>
inline mv::ServiceProxy<T>::operator T* () const
{
	return *this->_service_location;
}


template <typename T>
inline T* mv::ServiceProxy<T>::operator->() const
{
	return *this->_service_location;
}

template <typename T>
inline T& mv::ServiceProxy<T>::operator*() const
{
	return **this->_service_location;
}
