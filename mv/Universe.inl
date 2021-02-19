#include "Universe.h"


template <typename ComponentType>
inline ComponentType* mv::Universe::get_component(mv::id_type component_id)
{
	return component_cast<ComponentType*>(this->_component_pool.get(component_id));
}


template <typename ComponentType, typename... Args>
inline ComponentType* mv::Universe::add_component(id_type entity_id, Args&&... args)
{
	// TODO: delay creation
	id_type id = this->_component_pool.create<ComponentType>(std::forward<Args>(args)...);
	Component* component = this->_component_pool.get(id);
	component->_id = id;
	component->_entity_id = entity_id;
	this->_init_stack.push_back(id);
	return reinterpret_cast<ComponentType*>(component);
}
