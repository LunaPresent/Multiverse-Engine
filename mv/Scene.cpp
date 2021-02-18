#include "pch.h"
#include "Scene.h"

#include <algorithm> // find

#include "SceneManager.h"


mv::Scene::Scene()
	: _material_buckets(), _view{ mat4f::identity() }, _proj{ mat4f::identity() }
{}


void mv::Scene::add(const SceneObject& scene_object)
{
	bucket_list_type::iterator it = this->_material_buckets.find(scene_object.material_id());
	if (it == this->_material_buckets.end()) {
		it = this->_material_buckets.insert(Bucket{scene_object.material_id()}).first;
	}
	it->scene_object_ids.push_back(scene_object.id());
}

void mv::Scene::remove(const SceneObject& scene_object)
{
	bucket_list_type::iterator it = this->_material_buckets.find(scene_object.material_id());
	std::size_t obj_idx = std::find(it->scene_object_ids.begin(), it->scene_object_ids.end(), scene_object.id()) - it->scene_object_ids.begin();
	it->scene_object_ids[obj_idx] = it->scene_object_ids.back();
	it->scene_object_ids.pop_back();
	if (it->scene_object_ids.empty()) {
		this->_material_buckets.erase(it);
	}
}


mv::Scene::bucket_list_type::const_iterator mv::Scene::begin() const
{
	return this->_material_buckets.begin();
}

mv::Scene::bucket_list_type::const_iterator mv::Scene::end() const
{
	return this->_material_buckets.end();
}


void mv::Scene::set_proj_perspective(float view_angle, float aspect_ratio, float near, float far)
{
	this->_proj = mat4f::perspective(view_angle, aspect_ratio, near, far);
}

void mv::Scene::set_proj_ortho(float width, float height, float near, float far)
{
	this->_proj = mat4f::ortho(width, height, near, far);
}


const mv::mat4f& mv::Scene::view() const
{
	return this->_view;
}

const mv::mat4f& mv::Scene::proj() const
{
	return this->_proj;
}




bool mv::Scene::Bucket::operator<(const Bucket& rhs) const
{
	return this->material_id < rhs.material_id;
}

bool mv::Scene::Bucket::operator<(id_type rhs) const
{
	return this->material_id < rhs;
}

bool mv::operator<(id_type lhs, const Scene::Bucket& rhs)
{
	return lhs < rhs.material_id;
}
