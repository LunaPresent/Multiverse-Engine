#include "pch.h"
#include "CameraComponent.h"

#include "SceneManager.h"
#include "Entity.h"
#include "Universe.h"


mv::CameraComponent::CameraComponent()
	: _transform{ nullptr }
{}


void mv::CameraComponent::on_create()
{
	this->_transform = this->entity().add_component<TransformComponent>();
}

void mv::CameraComponent::on_destroy()
{
	this->entity().remove_component(this->_transform);
}


void mv::CameraComponent::late_update(float)
{
	SceneManager::instance().scene(this->universe().scene_id()).set_view_direction(
		this->_transform->get_position_absolute(), vec3f{ 0.f, 0.f, 1.f }, vec3f{ 0.f, 1.f, 0.f });
	SceneManager::instance().scene(this->universe().scene_id()).set_proj_ortho(100.f, 100.f, -1.f, 1.f);
}


mv::TransformComponent* mv::CameraComponent::transform() const
{
	return this->_transform;
}
