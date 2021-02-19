#include "Entity.h"

#include "Component.h"
#include "Universe.h"
#include "Multiverse.h"



template <typename ComponentType>
inline ComponentType* mv::Entity::component() const
{
	ComponentType* component = nullptr;
	for (id_type component_id : this->_component_ids) {
		component = this->universe().get_component<ComponentType>(component_id);
		if (component)
			break;
	}
	return component;
}

template <typename ComponentType>
inline std::vector<ComponentType*> mv::Entity::components() const
{
	std::vector<ComponentType*> components;
	components.reserve(this->_component_ids.size());
	for (id_type component_id : this->_component_ids) {
		ComponentType* component = this->universe().get_component<ComponentType>(component_id);
		if (component)
			components.push_back(component);
	}
	return components;
}


template <typename ComponentType, typename... Args>
inline ComponentType* mv::Entity::add_component(Args&&... args)
{
	ComponentType* component = this->universe().add_component<ComponentType>(this->_id, std::forward<Args>(args)...);
	this->_component_ids.push_back(static_cast<Component*>(component)->id());
	return component;
}
