#pragma once
#include "Resource.h"

#include <string>

namespace mv
{
	class Texture final : public Resource
	{
	private:
		uint _texture_handle;
		uint _width;
		uint _height;

	public:
		Texture();

		void set_data(uint texture_handle, uint width, uint height);

		uint texture_handle() const;
		uint width() const;
		uint height() const;

	private:
		void _unload() override;
	};

	class TextureFileLoader final : public ResourceLoader<Texture>
	{
	private:
		std::string _file_path;

	public:
		TextureFileLoader(const std::string& file_path);
		TextureFileLoader(std::string&& file_path);

	private:
		void load(Texture* resource) override;
	};
}
