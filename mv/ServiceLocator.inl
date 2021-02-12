#include "ServiceLocator.h"

#include "TemplateUtils.h"


template <typename... ServiceTypes>
inline mv::ServiceLocator<ServiceTypes...>::~ServiceLocator()
{
	this->cleanup();
}


template <typename... ServiceTypes>
template <typename ServiceBase, typename T, typename... Args>
inline void mv::ServiceLocator<ServiceTypes...>::set(Args&&... args)
{
	ServiceBase*& service = std::get<IndexOf<ServiceBase, ServiceTypes...>::value>(this->_services);
	delete service;
	service = new T(std::forward<Args>(args)...);
}

template <typename... ServiceTypes>
template <typename ServiceBase>
inline mv::ServiceProxy<ServiceBase> mv::ServiceLocator<ServiceTypes...>::get() const
{
	return ServiceProxy<ServiceBase>(&std::get<IndexOf<ServiceBase, ServiceTypes...>::value>(this->_services));
}


template <typename... ServiceTypes>
template <mv::uint i, typename std::enable_if<i == sizeof...(ServiceTypes), int>::type>
inline void mv::ServiceLocator<ServiceTypes...>::cleanup()
{}

template <typename... ServiceTypes>
template <mv::uint i, typename std::enable_if<i < sizeof...(ServiceTypes), int>::type>
	inline void mv::ServiceLocator<ServiceTypes...>::cleanup()
	{
		auto& p = std::get<sizeof...(ServiceTypes) - i - 1>(this->_services); // cleanup back to front
		delete p;
		p = nullptr;
		this->cleanup<i + 1>();
	}
