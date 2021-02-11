#pragma once
#include "setup.h"

#include <iterator> // iterator, random_access_iterator_tag
#include <map> // map
#include <type_traits> // enable_if, is_base_of
#include <vector> // vector

#include "Multiverse.h"

namespace mv
{
	class Component;
	class Universe;

	class Entity final
	{
		friend Multiverse;
		friend Universe;

	private:
		struct ComponentRef
		{
			id_type type_id;
			id_type component_id;
		};

	public:
		template <typename ComponentType>
		class ComponentList;

		template <typename ComponentType>
		class ComponentIterator : public std::iterator<std::random_access_iterator_tag, ComponentType>
		{
			friend ComponentList<ComponentType>;

		public:
			using component_type = ComponentType;

		private:
			const ComponentRef* _component_ref_it;
			id_type _universe_id;

			ComponentIterator(const ComponentRef* component_ref, id_type universe_id);
		public:
			ComponentIterator() = default;

			bool operator==(const ComponentIterator<ComponentType>& rhs) const;
			bool operator!=(const ComponentIterator<ComponentType>& rhs) const;
			bool operator<(const ComponentIterator<ComponentType>& rhs) const;
			bool operator>(const ComponentIterator<ComponentType>& rhs) const;
			bool operator<=(const ComponentIterator<ComponentType>& rhs) const;
			bool operator>=(const ComponentIterator<ComponentType>& rhs) const;

			ComponentType& operator*() const;
			ComponentType* operator->() const;

			ComponentIterator<ComponentType>& operator++();
			ComponentIterator<ComponentType> operator++(int);
			ComponentIterator<ComponentType>& operator--();
			ComponentIterator<ComponentType> operator--(int);

			ComponentIterator<ComponentType>& operator+=(std::ptrdiff_t n);
			ComponentIterator<ComponentType> operator+(std::ptrdiff_t n) const;
			ComponentIterator<ComponentType>& operator-=(std::ptrdiff_t n);
			ComponentIterator<ComponentType> operator-(std::ptrdiff_t n) const;
			std::ptrdiff_t operator-(const ComponentIterator<ComponentType>& rhs) const;

			ComponentType& operator[](std::ptrdiff_t i) const;
		};

		template <typename ComponentType>
		class ComponentList
		{
		public:
			using component_type = ComponentType;

		private:
			ComponentRef* _component_refs;
			size_type _component_count;
			id_type _entity_id;


			ComponentList(ComponentRef* component_refs, size_type count, id_type entity_id);
		public:
			ComponentList(const ComponentList<ComponentType>& other);
			ComponentList(ComponentList<ComponentType>&& other) noexcept;

			~ComponentList();

			ComponentList<ComponentType>& operator=(const ComponentList<ComponentType>& other);
			ComponentList<ComponentType>& operator=(ComponentList<ComponentType>&& other) noexcept;

			id_type type_id() const;

			ComponentIterator<ComponentType> begin() const;
			ComponentIterator<ComponentType> end() const;

			size_type size() const;
			bool empty() const;

			ComponentType& operator[](mv::size_type i) const;
			ComponentType& at(mv::size_type i) const;
		};


	private:
		id_type _id; // id of this entity, unique in the multiverse
		id_type _universe_id; // id of the universe in which the entity exists

		std::vector<ComponentRef> _component_refs;


		Entity(id_type id, id_type universe_id);

	public:
		Entity(const Entity&) = delete;
		Entity(Entity&& other) noexcept;

		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&& other) noexcept;

		/**
			\brief get entity id
		*/
		id_type id() const;
		/**
			\brief get universe id
		*/
		id_type universe_id() const;
		/**
			\brief get universe
		*/
		Universe& universe() const;

		/**
			\brief get component of type
			\returns an attached component of the specified type

			Get a component of the specified type, if multiple components of that type are attached to this entity,
			there is no guarantee which one it will return. If no component of that type was found this throws an exception.
		*/
		template <typename ComponentType>
		ComponentType& component() const;
		/**
			\brief get all components of type
			\returns a list of all attached components of the specified type
		*/
		template <typename ComponentType>
		ComponentList<ComponentType> components() const;
		/**
			\brief get component of type
			\returns pointer to an attached component of the specified type, nullptr if not found

			Get a pointer to a component of the specified type, if multiple components of that type are attached to this entity,
			there is no guarantee which one it will return. If no component of that type was found this returns nullptr.
		*/
		template <typename ComponentType>
		ComponentType* find_component() const;

		template <typename ComponentType, typename... Args>
		ComponentType& add_component(Args&&... args);
		template <typename ComponentType>
		bool remove_component(id_type component_id);
		bool remove_component(id_type type_id, id_type component_id);
	};

	template <typename ComponentType>
	typename Entity::template ComponentIterator<ComponentType> operator+(
		std::ptrdiff_t n, const typename Entity::template ComponentIterator<ComponentType>& it);
}

#include "Entity.inl"
