#pragma once
#include "setup.h"

#include <map>

#include "Hash.h"
#include "MemoryPool.h"


// place inside component class definition to auto generate all members it needs for RTTI
#define MV_COMPONENT_HEADER(base_type)\
	friend class mv::Component;\
	friend class mv::MemoryPool<Component, mv::component_pool_block_size, mv::max_component_size>;\
	using base = base_type;\
	mv::id_type type_id() const override;

// used to generate type ids for components, to be called immediately after the class definition
#define MV_REGISTER_COMPONENT(component_type)\
	static_assert(mv::Component::has_base<component_type>, #component_type " has no member type \'base\'");\
	static_assert(mv::Component::is_valid<component_type>, #component_type " is not a valid component type");\
	constexpr mv::id_type mv::Component::TypeID<component_type>::id(){ return mv::Hash::crc32(#component_type); }\
	inline mv::id_type component_type::type_id() const{ return mv::Component::type_id<component_type>(); }\
	template <> int mv::Component::_type_init<component_type> = mv::Component::register_component<component_type>();

namespace mv
{
	class Entity;
	class Universe;

	class Component
	{
		friend Universe;
		friend class Multiverse;
		friend class MemoryPool<Component, component_pool_block_size, max_component_size>;

	private:
		class ComponentManagerBase
		{
		public:
			virtual ~ComponentManagerBase() = default;

			virtual id_type type_id() const = 0;
			virtual id_type base_type_id() const = 0;
			virtual bool derives_from(id_type type_id) const = 0;
		};

		template <typename ComponentType>
		class ComponentManager final : public ComponentManagerBase
		{
		public:
			id_type type_id() const override;
			id_type base_type_id() const override;
			bool derives_from(id_type type_id) const override;
		};

		template <typename ComponentType>
		struct TypeID final
		{
			static constexpr id_type id(); // undefined on purpose
		};

	public:
		template <typename ComponentType, typename = void>
		static constexpr bool has_base = false;
		template <typename ComponentType>
		static constexpr bool has_base<ComponentType, std::void_t<decltype(sizeof(typename ComponentType::base))>> = true;

		/** ComponentType needs to have a member type "base" with the following properties:
			* "mv::Component" is a valid base type of "base"
			* "base" is a valid base type of "ComponentType"
			* "base" is not the same type as "ComponentType"
		*/
		template <typename ComponentType>
		static constexpr bool is_valid = std::is_base_of<Component, typename ComponentType::base>::value &&
			std::is_base_of<typename ComponentType::base, ComponentType>::value &&
			!std::is_same<typename ComponentType::base, ComponentType>::value;
		template <>
		static constexpr bool is_valid<Component> = true;

	private:
		template <typename ComponentType>
		static int _type_init; // needed to allow registering the component outside of a scope
		static int _base_component_type_init; // used for mv::Component itself, as it is defined in header
		static std::map<id_type, ComponentManagerBase*> _component_managers;
		static constexpr id_type _valid_flag_mask = 0x80000000; // bit indicates if component has been initialised
		static constexpr id_type _enabled_flag_mask = 0x40000000; // bit indicates if component will update this frame
		static constexpr id_type _component_enabled_flag_mask = 0x20000000; // bit indicates if component is set to enabled
		static constexpr id_type _entity_enabled_flag_mask = 0x10000000; // bit indicates if parent entity is set to enabled
		static constexpr id_type _destroy_flag_mask = 0x08000000; // bit indicates if component is slated to be destroyed
		static constexpr id_type _entity_id_mask = 0x07FFFFFF;

		id_type _id; // id of component, unique per component type in the multiverse
		id_type _status_flags_and_entity_id; // status flags and id of owning entity bitfield


		static ComponentManagerBase* _get_manager(id_type component_type_id);

		static void cleanup();

	public:
		// do not call directly, use MV_REGISTER_COMPONENT macro instead
		template <typename ComponentType>
		static int register_component();
		static bool is_base_of(id_type base_id, id_type derived_id);
		template <typename ComponentType, typename std::enable_if<is_valid<ComponentType>, int>::type = 0>
		static constexpr id_type type_id();

	protected:
		Component();
		virtual ~Component() = default;

	public:
		virtual id_type type_id() const;

		id_type id() const { return this->_id; }
		id_type entity_id() const { return this->_status_flags_and_entity_id & _entity_id_mask; }
		id_type universe_id() const;
		Entity& entity() const;
		Universe& universe() const;

		bool valid() const { return this->_status_flags_and_entity_id & _valid_flag_mask; }
		bool enabled() const { return this->_status_flags_and_entity_id & _enabled_flag_mask; }

	private:
		/**
			\brief initialise component

			Called at the start of the first frame after this component was added.
		*/
		virtual void on_create() {}
		/**
			\brief deinitialise component

			Called at the start of the first frame after this component was slated to be removed.
			If any components are added during this stage, their call to on_create
			will happen at the start of the next frame
		*/
		virtual void on_destroy() {}

		virtual void on_enable() {}
		virtual void on_disable() {}

		virtual void fixed_update(float) {}
		virtual void update(float) {}
		virtual void late_update(float) {}

		void handle_on_create();
		void handle_on_destroy();
		void handle_enabled_state();
	};

	template <typename ToType, typename FromType, typename std::enable_if<
		std::is_pointer<ToType>::value && std::is_pointer<FromType>::value &&
		!(!std::is_const<typename std::remove_pointer<ToType>::type>::value &&
		std::is_const<typename std::remove_pointer<FromType>::type>::value) &&
		std::is_base_of<Component, typename std::remove_pointer<ToType>::type>::value &&
		std::is_base_of<Component, typename std::remove_pointer<FromType>::type>::value, int>::type = 0>
	ToType component_cast(FromType component)
	{
		return Component::is_base_of(Component::type_id<typename std::remove_pointer<ToType>::type>(), component->type_id()) ?
			reinterpret_cast<ToType>(component) : nullptr;
	}
}

#include "Component.inl"
