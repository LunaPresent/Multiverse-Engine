#pragma once
#include "Component.h"

namespace mv
{
	class CameraComponent : public Component
	{
	public:
		using base = Component;

		void pre_render(float delta_time);
	};
}

MV_REGISTER_COMPONENT(mv::CameraComponent)
