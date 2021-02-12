#pragma once
#include "Input.h"

namespace mv
{
	class InputHandlerSDL final : public InputService
	{
	public:
		bool update() override;
	};
}
