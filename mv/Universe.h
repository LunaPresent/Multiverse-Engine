#pragma once
#include "setup.h"

#include <vector>
#include <map>

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
		class ComponentUpdaterBase
		{
		public:
			ComponentUpdaterBase() = default;
			virtual ~ComponentUpdaterBase() = default;

			virtual id_type type_id() const = 0;
			virtual std::size_t size() const = 0;

			virtual Component& at(std::size_t i) = 0;
			virtual const Component& at(std::size_t i) const = 0;

			virtual Component& get(id_type id) = 0;
			virtual const Component& get(id_type id) const = 0;

			virtual void remove(id_type id) = 0;

			virtual void update(float deltaTime) = 0;
			virtual void pre_render(float deltaTime) = 0;
		};

		template <typename ComponentType>
		class ComponentUpdater final : public ComponentUpdaterBase
		{
			std::vector<ComponentType> _components;
			static std::vector<uint> _lookup; // maps component id to index in _components
			static std::vector<id_type> _freed_ids; // constant time access for freed component ids

		public:
			ComponentUpdater() = default;

			id_type type_id() const override;
			std::size_t size() const override;

			Component& at(std::size_t i) override;
			const Component& at(std::size_t i) const override;

			Component& get(id_type id) override;
			const Component& get(id_type id) const override;

			template <typename... Args>
			ComponentType& add(Args&&... args);
			void remove(id_type id) override;

			void update(float deltaTime) override;
			void pre_render(float deltaTime) override;
		};

		class ComponentUpdaterList final
		{
			std::vector<ComponentUpdaterBase*> _updaters;
			std::map<id_type, uint> _lookup; // updater index per component type

		public:
			ComponentUpdaterList() = default;
			ComponentUpdaterList(const ComponentUpdaterList&) = delete;
			ComponentUpdaterList(ComponentUpdaterList&& other) noexcept;

			~ComponentUpdaterList();

			ComponentUpdaterList& operator=(const ComponentUpdaterList&) = delete;
			ComponentUpdaterList& operator=(ComponentUpdaterList&& other) noexcept;

			ComponentUpdaterBase* const* begin() const;
			ComponentUpdaterBase* const* end() const;

			Component& get(id_type type_id, id_type component_id) const;

			template <typename ComponentType, typename... Args>
			ComponentType& add(Args&&... args);
			void remove(id_type type_id, id_type component_id);
		};


		id_type _id;

		ComponentUpdaterList _updaters;

		float _update_interval;
		float _render_interval;
		float _update_timeout;
		float _render_timeout;
		bool _update_enabled;
		bool _render_enabled;


		Universe(id_type id);

		template <typename ComponentType>
		ComponentType& get_component(id_type component_id) const;
		template <typename ComponentType>
		ComponentType& get_component(id_type type_id, id_type component_id) const;
		Component& get_component(id_type type_id, id_type component_id) const;

		template <typename ComponentType, typename... Args>
		ComponentType& add_component(id_type entity_id, Args&&... args);
		void remove_component(id_type type_id, id_type component_id);

		void update(float delta_time);
		void pre_render(float delta_time);

	public:
		Universe(const Universe&) = delete;
		Universe(Universe&& other) noexcept;

		Universe& operator=(const Universe&) = delete;
		Universe& operator=(Universe&& other) noexcept;

		/**
			\brief get universe id
		*/
		id_type id() const;

		id_type spawn_entity() const;
		// todo: remove entity, transfer entity (to different universe)

		void set_update_interval(float interval);
		void set_update_enabled(bool enabled);
		void set_render_interval(float interval);
		void set_render_enabled(bool enabled);
	};
}

#include "Universe.inl"
