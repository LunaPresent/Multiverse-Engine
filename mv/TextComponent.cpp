#include "pch.h"
#include "TextComponent.h"

#include "Universe.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Mesh.h"

mv::TextComponent::TextComponent(id_type font_id)
{
	this->_mesh_id = ResourceManager::instance().create<Mesh>(new MeshTextLoader(font_id, this->_text_update_event.ref()), "text_mesh");
	ResourceManager::instance().load(this->_mesh_id);
}

mv::TextComponent::TextComponent(const std::string& font_name)
	: TextComponent(ResourceManager::instance().get_id(font_name))
{}


void mv::TextComponent::on_create()
{
	SceneManager::instance().create_scene_object(this->universe().scene_id(),
		ResourceManager::instance().get_id("text_material.mvm"), this->_mesh_id);
}


void mv::TextComponent::set_text(const std::string& text)
{
	this->_text_update_event.raise(text);
}
