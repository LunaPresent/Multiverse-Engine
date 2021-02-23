#include "pch.h"
#include "Entity.h"

#include <algorithm>	// find

#include "Universe.h"
#include "Component.h"


mv::Entity::Entity(id_type id, id_type universe_id)
	: _id{ id }, _universe_id{ universe_id }, _component_ids()

{}

mv::Entity::Entity(Entity&& other) noexcept
	: _id{ other._id }, _universe_id{ other._universe_id }, _component_ids(std::move(other._component_ids))
{
	other._id = 0;
	other._universe_id = 0;
}


mv::Entity& mv::Entity::operator=(Entity&& other) noexcept
{
	if (this == &other)
		return *this;
	this->_id = other._id;
	this->_universe_id = other._universe_id;
	this->_component_ids = std::move(other._component_ids);
	other._id = 0;
	other._universe_id = 0;
	return *this;
}


inline mv::id_type mv::Entity::id() const
{
	return this->_id;
}

mv::id_type mv::Entity::universe_id() const
{
	return this->_universe_id;
}

mv::Universe& mv::Entity::universe() const
{
	return mv::multiverse().universe(this->_universe_id);
}


void mv::Entity::remove_component(Component* component)
{
	this->remove_component(component->id());
}

void mv::Entity::remove_component(id_type component_id)
{
	*std::find(this->_component_ids.begin(), this->_component_ids.end(), component_id) = this->_component_ids.back();
	this->_component_ids.pop_back();
	this->universe().remove_component(component_id);
}
