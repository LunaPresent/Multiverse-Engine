#include "pch.h"
#include "ResourceManager.h"

#include <algorithm>
#include <filesystem>

#include "Multiverse.h"
#include "Resource.h"



mv::ResourceManager* mv::ResourceManager::_instance{ nullptr };



mv::ResourceManager::ResourceManager(const std::string& data_path)
	: _data_path{ std::filesystem::path(data_path).generic_string() }, _resources(), _aliases()
{
	this->_init();
	_instance = this;
}

mv::ResourceManager::~ResourceManager()
{
	_instance = nullptr;
	for (Resource* resource : this->_resources) {
		resource->unload();
	}
	for (Resource* resource : this->_resources) {
		delete resource;
	}
	this->_resources.clear();
	this->_aliases.clear();
}


mv::ResourceManager& mv::ResourceManager::instance()
{
	return *_instance;
}


mv::id_type mv::ResourceManager::get_id(const std::string& alias) const
{
	auto it = this->_aliases.find(alias);
	return (it != this->_aliases.end() ? it->second : invalid_id);
}


void mv::ResourceManager::_init()
{
	for (const std::filesystem::directory_entry& e : std::filesystem::recursive_directory_iterator(this->_data_path)) {
		if (e.is_regular_file()) {
			this->_register_resource(e.path().generic_string(), e.path().extension().generic_string());
		}
	}
}

void mv::ResourceManager::_register_resource(const std::string& path, const std::string& extension)
{
	std::string alias = path.substr(this->_data_path.size());
	id_type id;
	if (extension == ".jpg" || extension == ".png") {
		//id = this->add<Texture>(new FileResourceLoader<Texture>(path));
	}
	else if (extension == ".otf" || extension == ".ttf") {
		// load font
	}
	this->_aliases.emplace(std::move(alias), id);
}
