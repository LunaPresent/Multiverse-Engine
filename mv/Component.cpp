#include "pch.h"
#include "Component.h"

#include "Entity.h"
#include "Multiverse.h"
#include "Universe.h"



mv::Component::ComponentManagerBase* mv::Component::_get_manager(id_type component_type_id)
{
	return _component_managers[component_type_id];
}


void mv::Component::cleanup()
{
	for (auto e : _component_managers) {
		delete e.second;
	}
	_component_managers.clear();
}


bool mv::Component::is_base_of(id_type base_id, id_type derived_id)
{
	return _get_manager(derived_id)->derives_from(base_id);
}


mv::id_type mv::Component::type_id() const
{
	return type_id<Component>();
}


mv::id_type mv::Component::id() const
{
	return this->_id;
}

mv::id_type mv::Component::entity_id() const
{
	return this->_entity_id;
}

mv::id_type mv::Component::universe_id() const
{
	return this->entity().universe_id();
}

mv::Entity& mv::Component::entity() const
{
	return mv::multiverse().entity(this->_entity_id);
}

mv::Universe& mv::Component::universe() const
{
	return this->entity().universe();
}


void mv::Component::init()
{}

void mv::Component::fixed_update(float)
{}

void mv::Component::update(float)
{}

void mv::Component::late_update(float)
{}
