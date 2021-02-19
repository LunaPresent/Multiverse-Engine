#include "pch.h"
#include "Universe.h"

#include <algorithm> // find
#include <cmath>

#include "Entity.h"
#include "Multiverse.h"
#include "SceneManager.h"



mv::Universe::Universe(id_type id)
	: _id{ id },_scene_id{ invalid_id }, _component_pool(), _init_stack()
{
	this->_scene_id = SceneManager::instance().create_scene(true);
}

mv::Universe::Universe(Universe&& other) noexcept
	: _id{ other._id }, _scene_id{ other._scene_id },
	_component_pool(std::move(other._component_pool)), _init_stack(std::move(other._init_stack))
{
	other._id = invalid_id;
	other._scene_id = invalid_id;
}


mv::Universe::~Universe()
{
	if (this->_scene_id != invalid_id) {
		SceneManager::instance().destroy_scene(this->_scene_id);
	}
}


mv::Universe& mv::Universe::operator=(Universe&& other) noexcept
{
	if (this == &other)
		return *this;
	this->_id = other._id;
	this->_scene_id = other._scene_id;
	this->_component_pool = std::move(other._component_pool);
	this->_init_stack = std::move(other._init_stack);
	other._id = invalid_id;
	other._scene_id = invalid_id;
	return *this;
}


mv::id_type mv::Universe::id() const
{
	return this->_id;
}


mv::Component* mv::Universe::get_component(id_type component_id)
{
	return this->_component_pool.get(component_id);
}


void mv::Universe::remove_component(id_type component_id)
{
	// TODO: delay destruction
	this->_component_pool.destroy(component_id);
}


void mv::Universe::frame_start()
{
	for (id_type id : this->_init_stack) {
		this->_component_pool.get(id)->init();
	}
	this->_init_stack.clear();
}

void mv::Universe::fixed_update(float delta_time)
{
	for (Component* component : this->_component_pool) {
		component->fixed_update(delta_time);
	}
}

void mv::Universe::update(float delta_time)
{
	for (Component* component : this->_component_pool) {
		component->update(delta_time);
	}
	for (Component* component : this->_component_pool) {
		component->late_update(delta_time);
	}
}


mv::id_type mv::Universe::spawn_entity() const
{
	return mv::multiverse().create_entity(this->id());
}

mv::id_type mv::Universe::scene_id() const
{
	return this->_scene_id;
}
