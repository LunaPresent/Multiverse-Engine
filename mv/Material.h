#pragma once
#include "Resource.h"

#include <string>
#include <vector>

namespace mv
{
	class Material : public Resource
	{
	public:
		struct TextureLink
		{
			id_type texture_id;
			int sampler_location;
		};

	private:
		id_type _shader_id;
		int _model_location;
		int _view_location;
		int _proj_location;
		std::vector<TextureLink> _texture_links;

	public:
		void set_data(id_type shader_id, int model_location, int view_location, int proj_location,
			std::vector<TextureLink>&& texture_links);

		id_type shader_id() const;
		int model_location() const;
		int view_location() const;
		int proj_location() const;
		id_type texture_id(size_type i) const;
		int sampler_location(size_type i) const;
		size_type texture_count() const;

	private:
		void _unload() override;
	};


	class MaterialFileLoader final : public ResourceLoader<Material>
	{
	private:
		std::string _file_path;

	public:
		MaterialFileLoader(const std::string& file_path);
		MaterialFileLoader(std::string&& file_path);

	private:
		void load(Material* resource) override;
	};
}
