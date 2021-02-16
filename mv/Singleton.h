#pragma once
#include "setup.h"

namespace mv
{
	template <typename T>
	class Singleton
	{
	private:
		static T* _instance;

	public:
		static T& instance();
		static T* ptr();

	protected:
		Singleton();
		Singleton(const Singleton<T>&) = delete;
		Singleton(Singleton<T>&&) = delete;
		Singleton& operator=(const Singleton<T>&) = delete;
		Singleton& operator=(Singleton<T>&&) = delete;

		~Singleton();
	};
}

#include "Singleton.inl"
