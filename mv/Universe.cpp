#include "pch.h"
#include "Universe.h"

#include <algorithm> // find, swap
#include <cmath>

#include "Entity.h"
#include "Multiverse.h"
#include "SceneManager.h"




mv::Universe::Universe(id_type id)
	: _id{ id }, _scene_id{ 0 }, _components(),
	_component_added{ false }, _component_removed{ false }, _component_enabled_changed{ false }
{
	this->_scene_id = SceneManager::instance().create_scene(true);
}

mv::Universe::Universe(Universe&& other) noexcept
	: _id{ other._id }, _scene_id{ other._scene_id }, _components(std::move(other._components)),
	_component_added{ other._component_added }, _component_removed{ other._component_removed },
	_component_enabled_changed{ other._component_enabled_changed }
{
	other._id = 0;
	other._scene_id = 0;
}


mv::Universe::~Universe()
{
	if (this->_scene_id) {
		SceneManager::instance().destroy_scene(this->_scene_id);
		this->_scene_id = 0;
	}
}


mv::Universe& mv::Universe::operator=(Universe&& other) noexcept
{
	if (this == &other)
		return *this;
	this->_id = other._id;
	this->_scene_id = other._scene_id;
	this->_components = std::move(other._components);
	this->_component_added = other._component_added;
	this->_component_removed = other._component_removed;
	this->_component_enabled_changed = other._component_enabled_changed;
	other._id = 0;
	other._scene_id = 0;
	return *this;
}


mv::id_type mv::Universe::id() const
{
	return this->_id;
}


mv::Component* mv::Universe::get_component(id_type component_id)
{
	return this->_components.get(component_id);
}


void mv::Universe::remove_component(id_type component_id)
{
	this->disable_component(component_id);
	this->_component_removed = true;
	this->_components.get(component_id)->_status_flags_and_entity_id |= Component::_destroy_flag_mask;
}

void mv::Universe::enable_component(id_type component_id)
{
	Component* component = this->_components.get(component_id);
	this->_component_enabled_changed = !component->enabled();
	if (!(component->_status_flags_and_entity_id & Component::_destroy_flag_mask)) {
		component->_status_flags_and_entity_id |= Component::_component_enabled_flag_mask;
	}
}

void mv::Universe::disable_component(id_type component_id)
{
	Component* component = this->_components.get(component_id);
	this->_component_enabled_changed = component->enabled();
	component->_status_flags_and_entity_id &= ~Component::_component_enabled_flag_mask;
}


void mv::Universe::frame_start()
{
	while (this->_component_added) {
		this->_component_added = false;
		for (Component* component : this->_components) {
			if (component->type_id())
				component->handle_on_create();
		}
	}

	while (this->_component_enabled_changed) {
		this->_component_enabled_changed = false;
		for (Component* component : this->_components) {
			if (component->type_id())
				component->handle_enabled_state();
		}
	}

	if (this->_component_removed) {
		while (this->_component_removed) {
			this->_component_removed = false;
			for (Component* component : this->_components) {
				if (component->type_id())
					component->handle_on_destroy();
			}
		}
		for (Component* component : this->_components) {
			if (component->type_id() && (component->_status_flags_and_entity_id & Component::_destroy_flag_mask)) {
				this->_components.destroy(component->id());
			}
		}
	}
}

void mv::Universe::fixed_update(float delta_time)
{
	for (Component* component : this->_components) {
		if (component->enabled())
			component->fixed_update(delta_time);
	}
}

void mv::Universe::update(float delta_time)
{
	for (Component* component : this->_components) {
		if (component->enabled())
			component->update(delta_time);
	}
	for (Component* component : this->_components) {
		if (component->enabled())
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
