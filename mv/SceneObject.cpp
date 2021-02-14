#include "pch.h"
#include "SceneObject.h"

#include "SceneManager.h"



mv::SceneObject::SceneObject(id_type id, id_type scene_id, id_type material_id, id_type mesh_id)
	: _id{ id }, _scene_id{ scene_id }, _material_id{ material_id }, _mesh_id{ mesh_id }, _model_transform{ mat4f::identity() }
{}


void mv::SceneObject::set_material(id_type material_id)
{
	SceneManager::instance().set_scene_object_material(this->_id, material_id);
}


void mv::SceneObject::set_mesh(id_type mesh_id)
{
	this->_mesh_id = mesh_id;
}


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


mv::id_type mv::SceneObject::id() const
{
	return this->_id;
}

mv::id_type mv::SceneObject::scene_id() const
{
	return this->_scene_id;
}

mv::id_type mv::SceneObject::material_id() const
{
	return this->_material_id;
}

mv::id_type mv::SceneObject::mesh_id() const
{
	return this->_mesh_id;
}

const mv::mat4f& mv::SceneObject::model_transform() const
{
	return this->_model_transform;
}
