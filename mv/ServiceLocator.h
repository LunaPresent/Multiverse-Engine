#pragma once
#include "setup.h"

#include <tuple>

namespace mv
{
	template <typename T>
	class ServiceProxy;

	template <typename... ServiceTypes>
	class ServiceLocator
	{
	private:
		std::tuple<ServiceTypes*...> _services;

	public:
		ServiceLocator() = default;
		ServiceLocator(const ServiceLocator&) = delete;

		~ServiceLocator();

		ServiceLocator& operator=(const ServiceLocator&) = delete;

		template <typename ServiceBase, typename T, typename... Args>
		void set(Args&&... args);
		template <typename ServiceBase>
		ServiceProxy<ServiceBase> get() const;

		template <uint i = 0, typename std::enable_if<i == sizeof...(ServiceTypes), int>::type = 0>
		void cleanup();
		template <uint i = 0, typename std::enable_if<i < sizeof...(ServiceTypes), int>::type = 0>
			void cleanup();
	};
}

#include "ServiceLocator.inl"
