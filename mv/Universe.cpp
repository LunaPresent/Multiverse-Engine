#include "pch.h"
#include "Universe.h"

#include <algorithm> // find
#include <cmath>

#include "Entity.h"
#include "Multiverse.h"
#include "Component.h"

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
	: _id{ id }, _updaters(),
	_update_interval{ 0.f }, _update_timeout{ 0.f }, _render_interval{ 0.f }, _render_timeout{ 0.f },
	_update_enabled{ true }, _render_enabled{ true }
{}


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
	if (!this->_update_enabled || (this->_update_timeout -= delta_time) >= 0.f)
		return;

	this->_update_timeout += this->_update_interval;
	this->_update_timeout = this->_update_timeout >= 0.f ? this->_update_timeout : 0.f;


	for (ComponentUpdaterBase* updater : this->_updaters) {
		updater->update(delta_time);
	}
}

void mv::Universe::pre_render(float delta_time)
{
	if (!this->_render_enabled)
		return;

	if ((this->_render_timeout -= this->_render_interval) < 0.f) {
		this->_render_timeout += this->_render_interval;
		this->_render_timeout = this->_render_timeout >= 0.f ? this->_render_timeout : 0.f;

		// calculate renderer model transform matrices in parallel thread
		for (ComponentUpdaterBase* updater : this->_updaters) {
			updater->pre_render(delta_time);
		}
	}
}


mv::Universe::Universe(Universe&& other) noexcept
	: _id{ other._id }, _updaters(std::move(other._updaters)),
	_update_interval{ other._update_interval }, _render_interval{ other._render_interval },
	_update_timeout{ other._update_timeout }, _render_timeout{ other._render_timeout },
	_update_enabled{ other._update_enabled }, _render_enabled{ other._render_enabled }
{
	other._id = invalid_id;
}


mv::Universe& mv::Universe::operator=(Universe&& other) noexcept
{
	if (this == &other)
		return *this;
	this->_id = other._id;
	this->_updaters = std::move(other._updaters);
	this->_update_interval = other._update_interval;
	this->_render_interval = other._render_interval;
	this->_update_timeout = other._update_timeout;
	this->_render_timeout = other._render_timeout;
	this->_update_enabled = other._update_enabled;
	this->_render_enabled = other._render_enabled;
	other._id = invalid_id;
	return *this;
}


mv::id_type mv::Universe::id() const
{
	return this->_id;
}


mv::Entity& mv::Universe::spawn_entity() const
{
	return mv::multiverse().create_entity(this->id());
}


void mv::Universe::set_update_interval(float interval)
{
	this->_update_interval = interval >= 0.f ? interval : 0.f;
	this->_update_timeout = this->_update_interval < this->_update_timeout ? this->_update_interval : this->_update_timeout;
}

void mv::Universe::set_update_enabled(bool enabled)
{
	this->_update_enabled = enabled;
}

void mv::Universe::set_render_interval(float interval)
{
	this->_render_interval = interval >= 0.f ? interval : 0.f;
	this->_render_timeout = this->_render_interval < this->_render_timeout ? this->_render_interval : this->_render_timeout;
}

void mv::Universe::set_render_enabled(bool enabled)
{
	this->_render_enabled = enabled;
}
