#pragma once
#include "setup.h"

#include <string>

#include "Scancodes.h"
#include "ServiceProxy.h"

namespace mv
{
	enum class InputActionState : byte
	{
		held = 0b0001,
		pressed = 0b0010,
		released = 0b0100

	};

	class InputService
	{
	public:
		virtual ~InputService() {}

		/**
			\brief update input
			\returns true if exit signal was received
		*/
		virtual bool update() = 0;

		/*virtual id_type bind_action(const std::string& action_name, Scancode scancode...) = 0; ???
		virtual id_type bind_axis(const std::string& axis_name, float scale, Scancode scancode...) = 0; ???

		virtual InputActionState action_state(id_type action_id) = 0;
		virtual float axis_value(id_type axis_id) = 0;*/
	};

	extern const ServiceProxy<InputService> input;
}
