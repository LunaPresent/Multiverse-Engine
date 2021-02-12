#pragma once

namespace mv
{
	template <typename T>
	class ServiceProxy
	{
	private:
		T* const* _service_location;

	public:
		ServiceProxy(T* const* service_location);

		operator T* () const;

		T* operator->() const;
		T& operator*() const;
	};
}

#include "ServiceProxy.inl"
