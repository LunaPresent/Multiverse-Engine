#include "pch.h"
#include "Universe.h"

#include <algorithm> // find
#include <cmath>

#include "Entity.h"
#include "Multiverse.h"
#include "Component.h"
#include "SceneManager.h"

mv::Universe::ComponentUpdaterList::ComponentUpdaterList(ComponentUpdaterList&& other) noexcept
	: _updaters{ std::move(other._updaters) }, _lookup{ std::move(other._lookup) }
{}


mv::Universe::ComponentUpdaterList::~ComponentUpdaterList()
{
	for (ComponentUpdaterBase* updater : this->_updaters) {
		delete updater;
	}
	this->_updaters.clear();
}


typename mv::Universe::ComponentUpdaterList& mv::Universe::ComponentUpdaterList::operator=(ComponentUpdaterList&& other) noexcept
{
	if (this == &other)
		return *this;
	this->_updaters = std::move(other._updaters);
	this->_lookup = std::move(other._lookup);
	return *this;
}


typename mv::Universe::ComponentUpdaterBase* const* mv::Universe::ComponentUpdaterList::begin() const
{
	return this->_updaters.data();
}

typename mv::Universe::ComponentUpdaterBase* const* mv::Universe::ComponentUpdaterList::end() const
{
	return this->_updaters.data() + this->_updaters.size();
}


mv::Component& mv::Universe::ComponentUpdaterList::get(id_type type_id, id_type component_id) const
{
	return this->_updaters.at(this->_lookup.at(type_id))->get(component_id);
}


void mv::Universe::ComponentUpdaterList::remove(id_type type_id, id_type component_id)
{
	this->_updaters.at(this->_lookup.at(type_id))->remove(component_id);
}




mv::Universe::Universe(id_type id)
	: _id{ id }, _updaters(), _scene_id{ invalid_id }
{
	this->_scene_id = SceneManager::instance().create_scene(true);
}

mv::Universe::Universe(Universe&& other) noexcept
	: _id{ other._id }, _updaters(std::move(other._updaters)), _scene_id{ other._scene_id }
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
	this->_updaters = std::move(other._updaters);
	this->_scene_id = other._scene_id;
	other._id = invalid_id;
	other._scene_id = invalid_id;
	return *this;
}


mv::id_type mv::Universe::id() const
{
	return this->_id;
}


mv::Component& mv::Universe::get_component(id_type type_id, id_type component_id) const
{
	return this->_updaters.get(type_id, component_id);
}


void mv::Universe::remove_component(id_type type_id, id_type component_id)
{
	this->_updaters.remove(type_id, component_id);
}


void mv::Universe::update(float delta_time)
{
	for (ComponentUpdaterBase* updater : this->_updaters) {
		updater->init();
	}
	// cleanup to be removed components here ?
	// and then actually remove them before updating of course

	for (ComponentUpdaterBase* updater : this->_updaters) {
		updater->update(delta_time);
	}
}

void mv::Universe::pre_render(float delta_time)
{
	for (ComponentUpdaterBase* updater : this->_updaters) {
		updater->init();
	}

	for (ComponentUpdaterBase* updater : this->_updaters) {
		updater->pre_render(delta_time);
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
