#include "pch.h"
#include "CameraComponent.h"

#include "SceneManager.h"
#include "Universe.h"


void mv::CameraComponent::late_update(float)
{
	SceneManager::instance().scene(this->universe().scene_id()).set_proj_ortho(100.f, 100.f, -1.f, 1.f);
}
