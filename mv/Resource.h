#pragma once
#include "setup.h"

#include "ResourceLoader.h"

namespace mv
{
	class ResourceManager;

	class Resource
	{
		friend ResourceManager;

	public:
		enum class LoadState
		{
			unloaded, loading, loaded, unloading
		};

	private:
		id_type _id;
		LoadState _load_state;
		ResourceLoaderBase* _resource_loader;


	public:
		Resource();
		Resource(const Resource&) = delete;

		virtual ~Resource();

		Resource& operator=(const Resource&) = delete;

		id_type id() const;
		LoadState load_state() const;

	private:
		bool load();
		bool unload();
		//void async_load();
		//void async_unload();

		virtual void _unload() = 0;
	};
}
