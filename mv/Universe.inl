#include "Universe.h"
#include "Component.h"


template <typename ComponentType>
std::vector<mv::uint> mv::Universe::ComponentUpdater<ComponentType>::_lookup{};

template <typename ComponentType>
std::vector<mv::id_type> mv::Universe::ComponentUpdater<ComponentType>::_freed_ids{};




template <typename ComponentType>
inline mv::id_type mv::Universe::ComponentUpdater<ComponentType>::type_id() const
{
	return mv::Component::type_id<ComponentType>;
}

template <typename ComponentType>
inline std::size_t mv::Universe::ComponentUpdater<ComponentType>::size() const
{
	return this->_components.size();
}


template <typename ComponentType>
inline mv::Component& mv::Universe::ComponentUpdater<ComponentType>::at(std::size_t i)
{
	return this->_components.at(i);
}

template <typename ComponentType>
inline const mv::Component& mv::Universe::ComponentUpdater<ComponentType>::at(std::size_t i) const
{
	return this->_components.at(i);
}


template <typename ComponentType>
inline mv::Component& mv::Universe::ComponentUpdater<ComponentType>::get(id_type id)
{
	return this->_components.at(_lookup.at(id));
}

template <typename ComponentType>
inline const mv::Component& mv::Universe::ComponentUpdater<ComponentType>::get(id_type id) const
{
	return this->_components.at(_lookup.at(id));
}


template <typename ComponentType>
template <typename... Args>
inline ComponentType& mv::Universe::ComponentUpdater<ComponentType>::add(Args&&... args)
{
	id_type id;
	uint idx = static_cast<uint>(this->_components.size());
	if (_freed_ids.empty()) {
		id = static_cast<id_type>(_lookup.size());
		_lookup.push_back(idx);
	}
	else {
		id = _freed_ids.back();
		_freed_ids.pop_back();
		_lookup[id] = idx;
	}
	this->_components.emplace_back(std::forward<Args>(args)...);
	this->_components.back()._id = id;
	return this->_components.back();
}

template <typename ComponentType>
inline void mv::Universe::ComponentUpdater<ComponentType>::remove(id_type id)
{
	this->_components.at(_lookup.at(id)) = std::move(this->_components.back());
	this->_components.pop_back();
	_lookup.at(this->_components.at(_lookup.at(id)).id()) = _lookup.at(id); // update index of moved component
	_freed_ids.push_back(id);
}

template <typename ComponentType>
inline void mv::Universe::ComponentUpdater<ComponentType>::update(float deltaTime)
{
	for (ComponentType& component : this->_components) {
		component.update(deltaTime);
	}
}

template <typename ComponentType>
inline void mv::Universe::ComponentUpdater<ComponentType>::pre_render(float deltaTime)
{
	for (const ComponentType& component : this->_components) {
		component.pre_render(deltaTime);
	}
}




template <typename ComponentType, typename... Args>
inline ComponentType& mv::Universe::ComponentUpdaterList::add(Args&&... args)
{
	auto it = this->_lookup.find(mv::Component::type_id<ComponentType>);
	if (it == this->_lookup.end()) {
		it = this->_lookup.emplace(mv::Component::type_id<ComponentType>, static_cast<id_type>(this->_updaters.size())).first;
		this->_updaters.push_back(new ComponentUpdater<ComponentType>);
	}
	return reinterpret_cast<ComponentUpdater<ComponentType>*>(this->_updaters.at(it->second))->add(std::forward<Args>(args)...);
}




template <typename ComponentType>
inline ComponentType& mv::Universe::get_component(mv::id_type component_id) const
{
	return *reinterpret_cast<ComponentType*>(&this->_updaters.get(Component::type_id<ComponentType>, component_id));
}

template <typename ComponentType>
inline ComponentType& mv::Universe::get_component(id_type type_id, mv::id_type component_id) const
{
	return *reinterpret_cast<ComponentType*>(&this->_updaters.get(type_id, component_id));
}


template <typename ComponentType, typename... Args>
inline ComponentType& mv::Universe::add_component(Args&&... args)
{
	return this->_updaters.add<ComponentType>(std::forward<Args>(args)...);
}
