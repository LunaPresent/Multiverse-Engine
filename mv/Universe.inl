#include "Universe.h"


template <typename ComponentType>
inline ComponentType* mv::Universe::get_component(mv::id_type component_id)
{
	return component_cast<ComponentType*>(this->_components.get(component_id));
}


template <typename ComponentType, typename... Args>
inline ComponentType* mv::Universe::add_component(id_type entity_id, Args&&... args)
{
	this->_component_added = true;
	this->_component_enabled_changed = true;
	id_type id = this->_components.create<ComponentType>(std::forward<Args>(args)...);
	Component* component = this->_components.get(id);
	component->_id = id;
	component->_status_flags_and_entity_id |= entity_id & Component::_entity_id_mask;
	component->_status_flags_and_entity_id |= Component::_entity_enabled_flag_mask | Component::_component_enabled_flag_mask;
	return reinterpret_cast<ComponentType*>(component);
}
