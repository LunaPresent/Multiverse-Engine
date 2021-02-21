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


mv::Component::Component()
	: _id{ 0 }, _status_flags_and_entity_id{ 0 }
{}


mv::id_type mv::Component::type_id() const
{
	return type_id<Component>();
}


mv::id_type mv::Component::universe_id() const
{
	return this->entity().universe_id();
}

mv::Entity& mv::Component::entity() const
{
	return mv::multiverse().entity(this->_status_flags_and_entity_id & _entity_id_mask);
}

mv::Universe& mv::Component::universe() const
{
	return this->entity().universe();
}


void mv::Component::handle_on_create()
{
	if (!(this->_status_flags_and_entity_id & _valid_flag_mask) && !(this->_status_flags_and_entity_id & _destroy_flag_mask)) {
		this->_status_flags_and_entity_id |= _valid_flag_mask;
		this->on_create();
	}
}

void mv::Component::handle_on_destroy()
{
	if ((this->_status_flags_and_entity_id & _valid_flag_mask) && (this->_status_flags_and_entity_id & _destroy_flag_mask)) {
		this->on_destroy();
		this->_status_flags_and_entity_id &= ~_valid_flag_mask;
	}
}

void mv::Component::handle_enabled_state()
{
	bool should_enable = (this->_status_flags_and_entity_id & (_component_enabled_flag_mask | _entity_enabled_flag_mask)) ==
		(_component_enabled_flag_mask | _entity_enabled_flag_mask);
	bool is_enabled = this->_status_flags_and_entity_id & _enabled_flag_mask;

	if (should_enable && !is_enabled) {
		this->_status_flags_and_entity_id ^= _enabled_flag_mask;
		this->on_enable();
	}
	else if (!should_enable && is_enabled) {
		this->_status_flags_and_entity_id ^= _enabled_flag_mask;
		this->on_disable();
	}
}
