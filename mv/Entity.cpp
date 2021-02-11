#include "pch.h"
#include "Entity.h"
#include "Universe.h"
#include "Component.h"


mv::Entity::Entity(id_type id, id_type universe_id)
	: _id{ id }, _universe_id{ universe_id }, _component_refs()

{}

mv::Entity::Entity(Entity&& other) noexcept
	: _id{ other._id }, _universe_id{ other._universe_id }, _component_refs(std::move(other._component_refs))
{
	other._id = invalid_id;
	other._universe_id = invalid_id;
}


mv::Entity& mv::Entity::operator=(Entity&& other) noexcept
{
	if (this == &other)
		return *this;
	this->_id = other._id;
	this->_universe_id = other._universe_id;
	this->_component_refs = std::move(other._component_refs);
	other._id = invalid_id;
	other._universe_id = invalid_id;
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

bool mv::Entity::remove_component(id_type type_id, id_type component_id)
{
	for (std::size_t i = 0; i < this->_component_refs.size(); ++i) {
		if (this->_component_refs[i].type_id == type_id && this->_component_refs[i].component_id == component_id) {
			this->universe().remove_component(type_id, component_id);
			this->_component_refs[i] = this->_component_refs.back();
			this->_component_refs.pop_back();
			return true;
		}
	}
	return false;
}
