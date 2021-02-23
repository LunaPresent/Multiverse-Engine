#pragma once
#include "Component.h"

#include "TransformComponent.h"

namespace mv
{
	class CameraComponent : public Component
	{
		MV_COMPONENT_HEADER(Component)

	private:
		TransformComponent* _transform;

	protected:
		CameraComponent();
		~CameraComponent() = default;

	private:
		void on_create() override;
		void on_destroy() override;

		void late_update(float delta_time) override;

	public:
		TransformComponent* transform() const;
	};
}

MV_REGISTER_COMPONENT(mv::CameraComponent)
