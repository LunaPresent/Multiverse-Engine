#include "pch.h"
#include "Component.h"

#include "Entity.h"
#include "Multiverse.h"
#include "Universe.h"



mv::Component::ComponentManagerBase* mv::Component::_get_manager(id_type component_type_id)
{
	id_type module_id = component_type_id >> (8u * sizeof(id_type) - module_id_bits);
	id_type relative_type_id = component_type_id - (module_id << (8u * sizeof(id_type) - module_id_bits));
	return _modules[module_id][relative_type_id];
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




mv::Component::Module::~Module()
{
	for (ComponentManagerBase* manager : this->_managers) {
		delete manager;
	}
	this->_managers.clear();
}


mv::id_type mv::Component::Module::size() const
{
	return static_cast<id_type>(this->_managers.size());
}


void mv::Component::Module::add(ComponentManagerBase* manager)
{
	this->_managers.push_back(manager);
}

mv::Component::ComponentManagerBase* mv::Component::Module::operator[](id_type i) noexcept
{
	return this->_managers[i];
}
