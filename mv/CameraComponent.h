#pragma once
#include "Component.h"

namespace mv
{
	class CameraComponent : public Component
	{
		MV_COMPONENT_HEADER(Component)

	public:
		void late_update(float delta_time) override;
	};
}

MV_REGISTER_COMPONENT(mv::CameraComponent)
