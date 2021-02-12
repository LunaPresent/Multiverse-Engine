#include "pch.h"
#include "SceneObject.h"

#include "SceneManager.h"


void mv::SceneObject::set_position(const vec3f& position)
{
	this->_model_transform(0, 3) = position.x;
	this->_model_transform(1, 3) = position.y;
	this->_model_transform(2, 3) = position.z;
}

void mv::SceneObject::set_transform(const vec3f& t, const quatf& r, const vec3f& s)
{
	this->_model_transform = mat4f::transform(t, r, s);
}

void mv::SceneObject::set_transform(const vec3f& position, const quatf& rotation)
{
	this->set_transform(position, rotation, vec3f{ 1.f, 1.f, 1.f });
}

void mv::SceneObject::set_transform(const vec3f& position, const vec3f& scale)
{
	this->set_transform(position, quatf(), scale);
}

void mv::SceneObject::set_transform(const vec3f& position)
{
	this->set_transform(position, quatf(), vec3f{ 1.f, 1.f, 1.f });
}

const mv::mat4f& mv::SceneObject::get_transform() const
{
	return this->_model_transform;
}
