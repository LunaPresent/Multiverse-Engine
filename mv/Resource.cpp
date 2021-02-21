#include "pch.h"
#include "Resource.h"

#include "ResourceManager.h"


mv::Resource::Resource()
	: _id{ 0 }, _load_state{ LoadState::unloaded }, _resource_loader{ nullptr }
{}

mv::Resource::~Resource()
{
	delete this->_resource_loader;
	this->_resource_loader = nullptr;
}


mv::id_type mv::Resource::id() const
{
	return this->_id;
}

mv::Resource::LoadState mv::Resource::load_state() const
{
	return this->_load_state;
}


bool mv::Resource::load()
{
	if (this->_load_state != LoadState::unloaded)
		return false;
	this->_load_state = LoadState::loading;
	this->_resource_loader->load(this);
	this->_load_state = LoadState::loaded;
	return true;
}

bool mv::Resource::unload()
{
	if (this->_load_state != LoadState::loaded)
		return false;
	this->_load_state = LoadState::unloading;
	this->_unload();
	this->_load_state = LoadState::unloaded;
	return true;
}

/*void mv::Resource::async_load()
{
}

void mv::Resource::async_unload()
{
}*/
