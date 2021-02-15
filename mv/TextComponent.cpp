#include "pch.h"
#include "TextComponent.h"

#include "ResourceManager.h"
#include "Mesh.h"

mv::TextComponent::TextComponent(id_type font_id)
{
	id_type mesh_id = ResourceManager::instance().create<Mesh>(new MeshTextLoader(font_id, this->_text_update_event.ref()), "text_mesh");
	ResourceManager::instance().load(mesh_id);
}

mv::TextComponent::TextComponent(const std::string& font_name)
	: TextComponent(ResourceManager::instance().get_id(font_name))
{}


void mv::TextComponent::set_text(const std::string& text)
{
	this->_text_update_event.raise(text);
}
