#include "ResourceManager.h"


template <typename T, typename std::enable_if<std::is_base_of<mv::Resource, T>::value, int>::type>
mv::id_type mv::ResourceManager::create(ResourceLoader<T>* loader)
{
	Resource* resource = new T();
	resource->_id = this->_resources.next_id();
	resource->_resource_loader = loader;
	return this->_resources.insert(resource);
}

template <typename T, typename std::enable_if<std::is_base_of<mv::Resource, T>::value, int>::type>
const T* mv::ResourceManager::get(id_type id) const
{
	if (!this->_resources.is_reserved(id)) {
		return nullptr;
	}
	return dynamic_cast<const T*>(this->_resources.at(id));
}

template <typename T, typename std::enable_if<std::is_base_of<mv::Resource, T>::value, int>::type>
inline const T* mv::ResourceManager::get(const std::string& alias) const
{
	return this->get(this->get_id(alias));
}
