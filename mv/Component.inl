#include "Component.h"


inline mv::Component::Module mv::Component::_modules[1u << module_id_bits]{};



// register mv::Component itself
constexpr mv::id_type mv::Component::TypeID<mv::Component>::id()
{
	return 0;
}
inline int mv::Component::_base_component_type_init = mv::Component::register_component<mv::Component>();




template <typename ComponentType>
inline int mv::Component::register_component()
{
	constexpr id_type module_id = type_id<ComponentType> >> (8u * sizeof(id_type) - module_id_bits);
	constexpr id_type relative_type_id = type_id<ComponentType> - (module_id << (8u * sizeof(id_type) - module_id_bits));
	if (_modules[module_id].size() != relative_type_id) {
		throw "type id mismatch";
	}
	_modules[module_id].add(new ComponentManager<ComponentType>);
	return 0;
}




template <typename ComponentType>
inline mv::id_type mv::Component::ComponentManager<ComponentType>::type_id() const
{
	return Component::type_id<ComponentType>;
}

template <typename ComponentType>
inline mv::id_type mv::Component::ComponentManager<ComponentType>::base_type_id() const
{
	return Component::type_id<typename ComponentType::base>;
}
template <>
inline mv::id_type mv::Component::ComponentManager<mv::Component>::base_type_id() const
{
	return Component::type_id<Component>;
}

template <typename ComponentType>
inline bool mv::Component::ComponentManager<ComponentType>::derives_from(id_type type_id) const
{
	if (this->type_id() == type_id) {
		return true;
	}
	return Component::_get_manager(this->base_type_id())->derives_from(type_id);
}
template <>
inline bool mv::Component::ComponentManager<mv::Component>::derives_from(id_type type_id) const
{
	return type_id == this->type_id();
}
