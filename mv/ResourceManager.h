#pragma once
#include "Singleton.h"

#include <string>
#include <type_traits>	// enable_if, is_base_of
#include <unordered_map>

#include "IDList.h"

namespace mv
{
	class Resource;
	template <typename ResourceType>
	class ResourceLoader;

	class ResourceManager final : public Singleton<ResourceManager>
	{
		friend class Multiverse;

	private:
		std::string _data_path;
		IDList<Resource*> _resources;
		std::unordered_map<std::string, id_type> _aliases;


		ResourceManager(const std::string& data_path);

		~ResourceManager();

	public:
		const std::string& data_path() const;

		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		id_type create(ResourceLoader<T>* loader);
		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		id_type create(ResourceLoader<T>* loader, const std::string& alias);
		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		id_type create(ResourceLoader<T>* loader, std::string&& alias);
		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		const T* get(id_type id) const;
		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		const T* get(const std::string& alias) const;
		id_type get_id(const std::string& alias) const;

		bool load(id_type resource_id) const;
		bool unload(id_type resource_id) const;
		//void async_load(id_type resource_id) const;
		//void async_unload(id_type resource_id) const;
		bool load(const std::string& alias) const;
		bool unload(const std::string& alias) const;
		//void async_load(const std::string& alias) const;
		//void async_unload(const std::string& alias) const;

	private:
		void _init();
		void _register_resource(const std::string& path, const std::string& extension);
	};
}

#include "ResourceManager.inl"
