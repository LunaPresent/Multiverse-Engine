#include "pch.h"
#include "Texture.h"

#include <utility>	// move

#include <glad/glad.h>
#include <stb_image.h>


mv::Texture::Texture()
	: Resource(), _texture_handle{ 0 }, _width{}, _height{}
{}


void mv::Texture::set_data(uint texture_handle, uint width, uint height)
{
	this->_texture_handle = texture_handle;
	this->_width = width;
	this->_height = height;
}


mv::uint mv::Texture::texture_handle() const
{
	return this->_texture_handle;
}

mv::uint mv::Texture::width() const
{
	return this->_width;
}

mv::uint mv::Texture::height() const
{
	return this->_height;
}


void mv::Texture::_unload()
{
	glDeleteTextures(1, &this->_texture_handle);
	this->_texture_handle = 0;
}




mv::TextureFileLoader::TextureFileLoader(const std::string& file_path)
	: _file_path(file_path)
{}

mv::TextureFileLoader::TextureFileLoader(std::string&& file_path)
	: _file_path(std::move(file_path))
{}


void mv::TextureFileLoader::load(Texture* resource)
{
	uint texture_handle;
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	int w, h, channel_count;
	byte* data = stbi_load(this->_file_path.c_str(), &w, &h, &channel_count, 0);
	if (!data) {
		throw "failed to load texture";
	}

	int gl_format = 0;
	if (channel_count == 3) {
		gl_format = GL_RGB;
	}
	else if (channel_count == 4) {
		gl_format = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, gl_format, w, h, 0, gl_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	resource->set_data(texture_handle, w, h);
}
