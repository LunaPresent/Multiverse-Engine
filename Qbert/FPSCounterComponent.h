#pragma once
#include "mv/Component.h"

class FPSCounterComponent : public mv::Component
{
public:
	using base = mv::Component;

private:
	float _print_interval;
	float _print_timeout;

public:
	FPSCounterComponent(float print_interval = 1.f);

	void pre_render(float delta_time);
};

MV_REGISTER_COMPONENT(FPSCounterComponent)
