#pragma once
#include "setup.h"

#include <string>
#include <type_traits>	// enable_if, is_base_of
#include <unordered_map>

#include "IDList.h"

namespace mv
{
	class Resource;
	template <typename ResourceType>
	class ResourceLoader;

	class ResourceManager final
	{
		friend class Multiverse;

	private:
		static ResourceManager* _instance;

		std::string _data_path;
		IDList<Resource*> _resources;
		std::unordered_map<std::string, id_type> _aliases;


		ResourceManager(const std::string& data_path);
		ResourceManager(const ResourceManager&) = delete;

		~ResourceManager();

		ResourceManager& operator=(const ResourceManager&) = delete;

	public:
		static ResourceManager& instance();

		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		id_type create(ResourceLoader<T>* loader);
		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		const T* get(id_type id) const;
		template <typename T, typename std::enable_if<std::is_base_of<Resource, T>::value, int>::type = 0>
		const T* get(const std::string& alias) const;
		id_type get_id(const std::string& alias) const;

	private:
		void _init();
		void _register_resource(const std::string& path, const std::string& extension);
	};
}

#include "ResourceManager.inl"
