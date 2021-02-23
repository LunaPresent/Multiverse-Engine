#include "pch.h"
#include "TransformComponent.h"

mv::TransformComponent::TransformComponent()
	: _parent{ nullptr }, _position{ 0.f, 0.f, 0.f }
{}


void mv::TransformComponent::set_parent_absolute(TransformComponent* parent)
{
	this->_position = this->get_position_absolute();
	this->_parent = parent;
	if (this->_parent) {
		this->_position -= this->_parent->get_position_absolute();
	}
}

void mv::TransformComponent::set_parent_relative(TransformComponent* parent)
{
	this->_parent = parent;
}

mv::TransformComponent* mv::TransformComponent::get_parent() const
{
	return this->_parent;
}


void mv::TransformComponent::set_position_absolute(const vec3f& pos)
{
	if (this->_parent) {
		this->_position = pos - this->_parent->get_position_absolute();
	}
	else {
		this->_position = pos;
	}
}

void mv::TransformComponent::set_position_relative(const vec3f& pos)
{
	this->_position = pos;
}

mv::vec3f mv::TransformComponent::get_position_absolute()
{
	if (this->_parent) {
		return this->_parent->get_position_absolute() + this->_position;
	}
	else {
		return this->_position;
	}
}

const mv::vec3f& mv::TransformComponent::get_position_relative()
{
	return this->_position;
}
