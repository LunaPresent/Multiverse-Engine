#pragma once
#include "setup.h"

#include <map>

#include "Hash.h"

// used to generate type ids for components, to be called immediately after the class definition
#define MV_REGISTER_COMPONENT(component_type)\
	static_assert(mv::Component::has_base<component_type>, #component_type " has no member type \'base\'");\
	static_assert(mv::Component::is_valid<component_type>, #component_type " is not a valid component type");\
	constexpr mv::id_type mv::Component::TypeID<component_type>::id(){ return mv::Hash::crc32(#component_type); }\
	template <> int mv::Component::_type_init<component_type> = mv::Component::register_component<component_type>();

namespace mv
{
	class Entity;
	class Universe;

	class Component
	{
		friend Universe;
		friend class Multiverse;

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

		template <typename ComponentType, typename std::enable_if<is_valid<ComponentType>, int>::type = 0>
		static constexpr id_type type_id = TypeID<ComponentType>::id();

	private:
		template <typename ComponentType>
		static int _type_init; // needed to allow registering the component outside of a scope
		static int _base_component_type_init; // used for mv::Component itself, as it is defined in header
		static std::map<id_type, ComponentManagerBase*> _component_managers;

		id_type _id; // id of component, unique per component type in the multiverse
		id_type _entity_id; // id of owning entity


		static ComponentManagerBase* _get_manager(id_type component_type_id);

		static void cleanup();

	protected:
		Component() = default;

	public:
		// do not call directly, use MV_REGISTER_COMPONENT macro instead
		template <typename ComponentType>
		static int register_component();
		static bool is_base_of(id_type base_id, id_type derived_id);

		id_type id() const;
		id_type entity_id() const;
		id_type universe_id() const;
		Entity& entity() const;
		Universe& universe() const;

		void init();
		void update(float delta_time);
		void pre_render(float delta_time);
	};
}

#include "Component.inl"
