#include "pch.h"
#include "Component.h"

#include "Entity.h"
#include "Multiverse.h"
#include "Universe.h"



mv::Component::ComponentManagerBase* mv::Component::_get_manager(id_type component_type_id)
{
	return _component_managers[component_type_id];
}


bool mv::Component::is_base_of(id_type base_id, id_type derived_id)
{
	return _get_manager(derived_id)->derives_from(base_id);
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


void mv::Component::update(float)
{}

void mv::Component::pre_render(float)
{}
