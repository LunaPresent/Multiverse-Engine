#include "Entity.h"

#include "Component.h"
#include "Universe.h"
#include "Multiverse.h"


template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType>::ComponentIterator(const ComponentRef* component_ref, id_type universe_id)
	: _component_ref_it{ component_ref }, _universe_id{ universe_id }
{}


template <typename ComponentType>
inline bool mv::Entity::ComponentIterator<ComponentType>::operator==(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it == rhs._component_ref_it;
}

template <typename ComponentType>
inline bool mv::Entity::ComponentIterator<ComponentType>::operator!=(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it != rhs._component_ref_it;
}

template <typename ComponentType>
inline bool mv::Entity::ComponentIterator<ComponentType>::operator<(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it < rhs._component_ref_it;
}

template <typename ComponentType>
inline bool mv::Entity::ComponentIterator<ComponentType>::operator>(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it > rhs._component_ref_it;
}

template <typename ComponentType>
inline bool mv::Entity::ComponentIterator<ComponentType>::operator<=(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it <= rhs._component_ref_it;
}

template <typename ComponentType>
inline bool mv::Entity::ComponentIterator<ComponentType>::operator>=(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it >= rhs._component_ref_it;
}


template <typename ComponentType>
inline ComponentType& mv::Entity::ComponentIterator<ComponentType>::operator*() const
{
	return Multiverse::universe(this->_universe_id).get_component<ComponentType>(
		this->_component_ref_it->type_id, this->_component_ref_it->component_id);
}

template <typename ComponentType>
inline ComponentType* mv::Entity::ComponentIterator<ComponentType>::operator->() const
{
	return &this->operator*();
}


template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType>& mv::Entity::ComponentIterator<ComponentType>::operator++()
{
	++this->_component_ref_it;
	return *this;
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType> mv::Entity::ComponentIterator<ComponentType>::operator++(int)
{
	++this->_component_ref_it;
	return ComponentIterator<ComponentType>(this->_component_ref_it - 1, this->_universe_id);
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType>& mv::Entity::ComponentIterator<ComponentType>::operator--()
{
	--this->_component_ref_it;
	return *this;
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType> mv::Entity::ComponentIterator<ComponentType>::operator--(int)
{
	--this->_component_ref_it;
	return ComponentIterator<ComponentType>(this->_component_ref_it + 1, this->_universe_id);
}


template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType>& mv::Entity::ComponentIterator<ComponentType>::operator+=(std::ptrdiff_t n)
{
	this->_component_ref_it += n;
	return *this;
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType> mv::Entity::ComponentIterator<ComponentType>::operator+(std::ptrdiff_t n) const
{
	return ComponentIterator<ComponentType>(this->_component_ref_it + n, this->_universe_id);
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType>& mv::Entity::ComponentIterator<ComponentType>::operator-=(std::ptrdiff_t n)
{
	this->_component_ref_it -= n;
	return *this;
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType> mv::Entity::ComponentIterator<ComponentType>::operator-(std::ptrdiff_t n) const
{
	return ComponentIterator<ComponentType>(this->_component_ref_it - n, this->_universe_id);
}

template <typename ComponentType>
inline std::ptrdiff_t mv::Entity::ComponentIterator<ComponentType>::operator-(const ComponentIterator<ComponentType>& rhs) const
{
	return this->_component_ref_it - rhs._component_ref_it;
}


template <typename ComponentType>
inline ComponentType& mv::Entity::ComponentIterator<ComponentType>::operator[](std::ptrdiff_t i) const
{
	return *(*this + i);
}




template <typename ComponentType>
mv::Entity::ComponentList<ComponentType>::ComponentList(ComponentRef* component_refs, size_type count, id_type entity_id)
	: _component_refs{ component_refs }, _component_count{ count }, _entity_id{ entity_id }
{}

template <typename ComponentType>
mv::Entity::ComponentList<ComponentType>::ComponentList(const ComponentList<ComponentType>& other)
	: _component_refs(new ComponentRef[other._component_count]), _component_count{ other._component_count },
	_entity_id{ other._entity_id }
{
	for (size_type i = 0; i < this->_component_count; ++i) {
		this->_component_refs[i] = other._component_refs[i];
	}
}

template <typename ComponentType>
mv::Entity::ComponentList<ComponentType>::ComponentList(ComponentList<ComponentType>&& other) noexcept
	: _component_refs(other._component_refs), _component_count{ other._component_count }, _entity_id{ other._entity_id }
{
	other._component_refs = nullptr;
}


template <typename ComponentType>
mv::Entity::ComponentList<ComponentType>::~ComponentList()
{
	delete[] this->_component_refs;
	this->_component_refs = nullptr;
}


template <typename ComponentType>
inline mv::Entity::ComponentList<ComponentType>&
mv::Entity::ComponentList<ComponentType>::operator=(const ComponentList<ComponentType>& other)
{
	if (this == &other)
		return *this;
	if (other._component_count > this->_component_count) {
		delete[] this->_component_refs;
		this->_component_refs = new ComponentRef[other._component_count];
	}
	for (size_type i = 0; i < this->_component_count; ++i) {
		this->_component_refs[i] = other._component_refs[i];
	}
	this->_component_count = other._component_count;
	this->_entity_id = other._entity_id;
	return *this;
}

template <typename ComponentType>
inline mv::Entity::ComponentList<ComponentType>&
mv::Entity::ComponentList<ComponentType>::operator=(ComponentList<ComponentType>&& other) noexcept
{
	if (this == &other)
		return *this;
	delete[] this->_component_refs;
	this->_component_refs = other._component_refs;
	this->_component_count = other._component_count;
	this->_entity_id = other._entity_id;
	other._component_refs = nullptr;
	return *this;
}


template <typename ComponentType>
inline mv::id_type mv::Entity::ComponentList<ComponentType>::type_id() const
{
	return mv::Component::type_id<ComponentType>;
}


template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType> mv::Entity::ComponentList<ComponentType>::begin() const
{
	return ComponentIterator<ComponentType>(this->_component_ids,
		mv::multiverse().entity(this->_entity_id)._universe_id);
}

template <typename ComponentType>
inline mv::Entity::ComponentIterator<ComponentType> mv::Entity::ComponentList<ComponentType>::end() const
{
	return ComponentIterator<ComponentType>(this->_component_ids + this->_component_count,
		mv::multiverse().entity(this->_entity_id)._universe_id);
}


template <typename ComponentType>
inline mv::size_type mv::Entity::ComponentList<ComponentType>::size() const
{
	return this->_component_count;
}

template <typename ComponentType>
inline bool mv::Entity::ComponentList<ComponentType>::empty() const
{
	return this->_component_count != 0u;
}


template <typename ComponentType>
inline ComponentType& mv::Entity::ComponentList<ComponentType>::operator[](mv::size_type i) const
{
	return this->begin()[i];
}

template <typename ComponentType>
inline ComponentType& mv::Entity::ComponentList<ComponentType>::at(mv::size_type i) const
{
	return this->begin()[i];
}




template <typename ComponentType>
inline ComponentType& mv::Entity::component() const
{
	for (ComponentRef component_ref : this->_component_refs) {
		if (mv::Component::is_base_of(mv::Component::type_id<ComponentType>, component_ref.type_id)) {
			return this->universe().get_component(component_ref.type_id, component_ref.component_id);
		}
	}
	throw "component not found";
}

template <typename ComponentType>
inline mv::Entity::ComponentList<ComponentType> mv::Entity::components() const
{
	ComponentRef* component_refs = new ComponentRef[this->_component_refs.size()];
	size_type count = 0;
	for (ComponentRef component_ref : this->_component_refs) {
		if (mv::Component::is_base_of(mv::Component::type_id<ComponentType>, component_ref.type_id)) {
			component_refs[count] = component_ref;
			++count;
		}
	}
	return ComponentList<ComponentType>(component_refs, count, this->_universe_id);
}

template <typename ComponentType>
inline ComponentType* mv::Entity::find_component() const
{
	for (ComponentRef component_ref : this->_component_refs) {
		if (mv::Component::is_base_of(mv::Component::type_id<ComponentType>, component_ref.type_id)) {
			return reinterpret_cast<ComponentType*>(&this->universe().get_component(component_ref.type_id, component_ref.component_id));
		}
	}
	return nullptr;
}


template <typename ComponentType, typename... Args>
inline ComponentType& mv::Entity::add_component(Args&&... args)
{
	ComponentType& component = this->universe().add_component<ComponentType>(this->_id, std::forward<Args>(args)...);
	this->_component_refs.push_back(ComponentRef{ Component::type_id<ComponentType>, component.id() });
	return component;
}

template <typename ComponentType>
inline bool mv::Entity::remove_component(id_type component_id)
{
	return this->remove_component(Component::type_id<ComponentType>, component_id);
}




template <typename ComponentType>
typename mv::Entity::template ComponentIterator<ComponentType> mv::operator+(
	std::ptrdiff_t n, const typename mv::Entity::template ComponentIterator<ComponentType>& it)
{
	return it + n;
}
