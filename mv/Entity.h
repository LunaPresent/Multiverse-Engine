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
		id_type _id; // id of this entity, unique in the multiverse
		id_type _universe_id; // id of the universe in which the entity exists

		std::vector<id_type> _component_ids;


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
			\returns pointer to an attached component of the specified type, nullptr if not found

			Get a pointer to a component of the specified type, if multiple components of that type are attached to this entity,
			there is no guarantee which one it will return. If no component of that type was found this returns nullptr.
		*/
		template <typename ComponentType>
		ComponentType* component() const;
		/**
			\brief get all components of type
			\returns a list of pointers to all attached components of the specified type
		*/
		template <typename ComponentType>
		std::vector<ComponentType*> components() const;

		template <typename ComponentType, typename... Args>
		ComponentType* add_component(Args&&... args);
		void remove_component(Component* component);
		void remove_component(id_type component_id);
	};
}

#include "Entity.inl"
