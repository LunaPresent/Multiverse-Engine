#pragma once
#include "setup.h"

#include <vector>
#include <map>

#include "Component.h"
#include "MemoryPool.h"

namespace mv
{
	class Multiverse;
	class Component;
	class Entity;

	class Universe final
	{
		friend Multiverse;
		friend Entity;

	private:
		id_type _id;
		id_type _scene_id;
		MemoryPool<Component, component_pool_block_size, max_component_size> _components;
		bool _component_added;
		bool _component_removed;
		bool _component_enabled_changed;


		Universe(id_type id);
	public:
		Universe(const Universe&) = delete;
		Universe(Universe&& other) noexcept;

		~Universe();

		Universe& operator=(const Universe&) = delete;
		Universe& operator=(Universe&& other) noexcept;

		/**
			\brief get universe id
		*/
		id_type id() const;

	private:
		template <typename ComponentType>
		ComponentType* get_component(id_type component_id);
		Component* get_component(id_type component_id);

		template <typename ComponentType, typename... Args>
		ComponentType* add_component(id_type entity_id, Args&&... args);
		void remove_component(id_type component_id);
		void enable_component(id_type component_id);
		void disable_component(id_type component_id);

		void frame_start();
		void fixed_update(float delta_time);
		void update(float delta_time);

	public:
		id_type spawn_entity() const;
		// TODO: remove entity, transfer entity (to different universe)

		id_type scene_id() const;
	};
}

#include "Universe.inl"
