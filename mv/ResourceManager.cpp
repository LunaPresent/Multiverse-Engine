#include "pch.h"
#include "ResourceManager.h"

#include <algorithm>
#include <filesystem>

#include "Multiverse.h"
#include "Resource.h"
#include "Texture.h"
#include "Font.h"
#include "Shader.h"
#include "Material.h"



mv::ResourceManager::ResourceManager(const std::string& data_path)
	: _data_path(data_path.back() == '/' ? data_path : data_path + '/' ), _resources(), _aliases()
{
	this->_init();
}

mv::ResourceManager::~ResourceManager()
{
	for (Resource* resource : this->_resources) {
		resource->unload();
	}
	for (Resource* resource : this->_resources) {
		delete resource;
	}
	this->_resources.clear();
	this->_aliases.clear();
}


const std::string& mv::ResourceManager::data_path() const
{
	return this->_data_path;
}


mv::id_type mv::ResourceManager::get_id(const std::string& alias) const
{
	auto it = this->_aliases.find(alias);
	return (it != this->_aliases.end() ? it->second : 0);
}


bool mv::ResourceManager::load(id_type resource_id) const
{
	return this->_resources[resource_id]->load();
}

bool mv::ResourceManager::unload(id_type resource_id) const
{
	return this->_resources[resource_id]->unload();
}


bool mv::ResourceManager::load(const std::string& alias) const
{
	return this->_resources[this->get_id(alias)]->load();
}

bool mv::ResourceManager::unload(const std::string& alias) const
{
	return this->_resources[this->get_id(alias)]->unload();
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
	if (extension == ".jpg" || extension == ".png") {
		this->create<Texture>(new TextureFileLoader(path), std::move(alias));
	}
	else if (extension == ".otf" || extension == ".ttf") {
		this->create<Font>(new FontFileLoader(path), std::move(alias));
	}
	else if (extension == ".mvs") {
		this->create<Shader>(new ShaderFileLoader(path), std::move(alias));
	}
	else if (extension == ".mvm") {
		this->create<Material>(new MaterialFileLoader(path), std::move(alias));
	}
}
