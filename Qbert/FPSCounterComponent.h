#pragma once
#include "mv/Component.h"

class FPSCounterComponent : public mv::Component
{
	MV_COMPONENT_HEADER(mv::Component)

private:
	float _print_interval;
	float _print_timeout;
	unsigned int _frame_count;

protected:
	FPSCounterComponent(float print_interval = 1.f);
	~FPSCounterComponent() = default;

private:
	void update(float delta_time) override;
};

MV_REGISTER_COMPONENT(FPSCounterComponent)
