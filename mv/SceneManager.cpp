#include "pch.h"
#include "SceneManager.h"



mv::SceneManager::SceneManager()
{}

mv::SceneManager::~SceneManager()
{}


mv::id_type mv::SceneManager::create_scene(bool active)
{
	id_type id = this->_scenes.insert(Scene());
	if (active) {
		this->_active_scene_ids.insert(id);
	}
	return id;
}

void mv::SceneManager::destroy_scene(id_type id)
{
	this->_scenes.erase(id);
	this->_active_scene_ids.erase(id);
}

mv::id_type mv::SceneManager::create_scene_object(id_type scene_id, id_type material_id, id_type mesh_id)
{
	id_type id = this->_scene_objects.insert(SceneObject(this->_scene_objects.next_id(), scene_id, material_id, mesh_id));
	this->_scenes[scene_id].add(this->_scene_objects[id]);
	return id;
}

void mv::SceneManager::destroy_scene_object(id_type id)
{
	this->_scenes[this->_scene_objects[id].scene_id()].remove(this->_scene_objects[id]);
	this->_scene_objects.erase(id);
}


void mv::SceneManager::set_scene_active(id_type id)
{
	this->_active_scene_ids.insert(id);
}

void mv::SceneManager::set_scene_inactive(id_type id)
{
	this->_active_scene_ids.erase(id);
}


void mv::SceneManager::transfer_scene_object(id_type object_id, id_type destination_scene_id)
{
	SceneObject& obj = this->_scene_objects[object_id];
	this->_scenes[obj.scene_id()].remove(obj);
	obj._scene_id = destination_scene_id;
	this->_scenes[obj.scene_id()].add(obj);
}

void mv::SceneManager::set_scene_object_material(id_type object_id, id_type material_id)
{
	SceneObject& obj = this->_scene_objects[object_id];
	this->_scenes[obj.scene_id()].remove(obj);
	obj._material_id = material_id;
	this->_scenes[obj.scene_id()].add(obj);
}


mv::Scene& mv::SceneManager::scene(id_type id)
{
	return this->_scenes[id];
}

mv::SceneObject& mv::SceneManager::scene_object(id_type id)
{
	return this->_scene_objects[id];
}


const std::set<mv::id_type>& mv::SceneManager::active_scene_ids() const
{
	return this->_active_scene_ids;
}
