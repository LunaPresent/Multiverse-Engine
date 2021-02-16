#include "Singleton.h"


template <typename T>
T* mv::Singleton<T>::_instance{ nullptr };


template <typename T>
inline T& mv::Singleton<T>::instance()
{
	return *_instance;
}

template <typename T>
inline T* mv::Singleton<T>::ptr()
{
	return _instance;
}


template <typename T>
inline mv::Singleton<T>::Singleton()
{
	_instance = static_cast<T*>(this);
}


template <typename T>
inline mv::Singleton<T>::~Singleton()
{
	_instance = nullptr;
}
