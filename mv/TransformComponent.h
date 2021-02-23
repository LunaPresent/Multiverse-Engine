#pragma once
#include "Component.h"

#include "Vector.h"

namespace mv
{
	class TransformComponent : public Component
	{
		MV_COMPONENT_HEADER(Component)

	private:
		TransformComponent* _parent;
		vec3f _position;

	protected:
		TransformComponent();
		~TransformComponent() = default;

	public:
		// attach to new parent and keep absolute position
		void set_parent_absolute(TransformComponent* parent);
		// attach to new parent and keep absolute position
		void set_parent_relative(TransformComponent* parent);
		TransformComponent* get_parent() const;

		void set_position_absolute(const vec3f& pos);
		void set_position_relative(const vec3f& pos);
		vec3f get_position_absolute();
		const vec3f& get_position_relative();
	};
}

MV_REGISTER_COMPONENT(mv::TransformComponent)
