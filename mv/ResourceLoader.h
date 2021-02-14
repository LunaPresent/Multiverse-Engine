#pragma once
#include "setup.h"

#include <type_traits>

namespace mv
{
	class Resource;

	class ResourceLoaderBase
	{
	public:
		virtual ~ResourceLoaderBase() = default;

		virtual void load(Resource* resource) = 0;
	};

	template <typename ResourceType>
	class ResourceLoader : public ResourceLoaderBase
	{
	private:
		void load(Resource* resource) override final
		{
			this->load(reinterpret_cast<ResourceType*>(resource));
		}
		virtual void load(ResourceType* resource) = 0;
	};

	template <typename ResourceType>
	class FileResourceLoader;
}
