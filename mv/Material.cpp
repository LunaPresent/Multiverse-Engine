#include "pch.h"
#include "Material.h"

#include <fstream>	// ifstream
#include <utility>	// move

#include <json.h>

#include "ResourceManager.h"
#include "Shader.h"



void mv::Material::set_data(id_type shader_id, int model_location, int view_location, int proj_location,
	std::vector<TextureLink>&& texture_links)
{
	this->_shader_id = shader_id;
	this->_model_location = model_location;
	this->_view_location = view_location;
	this->_proj_location = proj_location;
	this->_texture_links = std::move(texture_links);
}


mv::id_type mv::Material::shader_id() const
{
	return this->_shader_id;
}

int mv::Material::model_location() const
{
	return this->_model_location;
}

int mv::Material::view_location() const
{
	return this->_view_location;
}

int mv::Material::proj_location() const
{
	return this->_proj_location;
}

mv::id_type mv::Material::texture_id(size_type i) const
{
	return this->_texture_links[i].texture_id;
}

int mv::Material::sampler_location(size_type i) const
{
	return this->_texture_links[i].sampler_location;
}

mv::size_type mv::Material::texture_count() const
{
	return static_cast<size_type>(this->_texture_links.size());
}


void mv::Material::_unload()
{
	this->_shader_id = invalid_id;
	this->_texture_links.clear();
}




mv::MaterialFileLoader::MaterialFileLoader(const std::string& file_path)
	: _file_path(file_path)
{}

mv::MaterialFileLoader::MaterialFileLoader(std::string&& file_path)
	: _file_path(std::move(file_path))
{}


void mv::MaterialFileLoader::load(Material* resource)
{
	std::ifstream f;

	f.open(this->_file_path);
	nlohmann::json j;
	f >> j;
	f.close();

	const Shader* shader = ResourceManager::instance().get<Shader>(j.find("shader")->get<std::string>());

	std::vector<Material::TextureLink> texture_links;
	auto j_textures = j.find("textures");
	texture_links.reserve(j_textures->size());
	for (auto& t : *j_textures) {
		id_type texture_id = ResourceManager::instance().get_id(t.find("texture")->get<std::string>());
		int sampler_location = shader->uniform_location(t.find("uniform")->get<std::string>());
		texture_links.push_back(Material::TextureLink{ texture_id, sampler_location });
	}

	auto j_model = j.find("model");
	auto j_view = j.find("view");
	auto j_proj = j.find("proj");
	std::string model_uniform_name = (j_model != j.end() ? j_model->get<std::string>() : "model");
	std::string view_uniform_name = (j_view != j.end() ? j_view->get<std::string>() : "view");
	std::string proj_uniform_name = (j_proj != j.end() ? j_proj->get<std::string>() : "proj");
	int model_location = shader->uniform_location(model_uniform_name);
	int view_location = shader->uniform_location(view_uniform_name);
	int proj_location = shader->uniform_location(proj_uniform_name);

	resource->set_data(shader->id(), model_location, view_location, proj_location, std::move(texture_links));
}
