#include "FPSCounterComponent.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include "mv/Entity.h"
#include "mv/TextComponent.h"

FPSCounterComponent::FPSCounterComponent(float print_interval)
	: _print_interval{ print_interval }, _print_timeout{ 0.f }, _frame_count{ 0u }
{}


void FPSCounterComponent::pre_render(float delta_time)
{
	++this->_frame_count;
	if ((this->_print_timeout -= delta_time) >= 0.f)
		return;

	this->_print_timeout += this->_print_interval;
	this->_print_timeout = this->_print_timeout >= 0.f ? this->_print_timeout : 0.f;

	std::cout << "fps: " << static_cast<int>(this->_frame_count / this->_print_interval) << std::endl;

	mv::TextComponent* textcomp = this->entity().find_component<mv::TextComponent>();
	if (textcomp) {
		textcomp->set_text(std::to_string(static_cast<int>(this->_frame_count / this->_print_interval)));
	}

	this->_frame_count = 0u;
}
