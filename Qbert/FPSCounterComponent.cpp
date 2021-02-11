#include "FPSCounterComponent.h"

#include <iostream>
#include <chrono>
#include <thread>

FPSCounterComponent::FPSCounterComponent(float print_interval)
	: _print_interval{ print_interval }, _print_timeout{ 0.f }
{}


void FPSCounterComponent::pre_render(float delta_time)
{
	// simulate work
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	if ((this->_print_timeout -= delta_time) >= 0.f)
		return;


	this->_print_timeout += this->_print_interval;
	this->_print_timeout = this->_print_timeout >= 0.f ? this->_print_timeout : 0.f;

	std::cout << "fps: " << static_cast<int>(1.f / delta_time) << std::endl;
}
