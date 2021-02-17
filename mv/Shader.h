#pragma once
#include "Resource.h"

#include <map>
#include <string>

namespace mv
{
	class Shader : public Resource
	{
	private:
		uint _program_handle;
		std::map<std::string, int> _uniform_locations;

	public:
		void set_data(uint program_handle, std::map<std::string, int>&& uniform_locations);

		uint program() const;
		int uniform_location(const std::string& uniform_name) const;

	private:
		void _unload() override;
	};


	class ShaderFileLoader final : public ResourceLoader<Shader>
	{
	private:
		std::string _file_path;

	public:
		ShaderFileLoader(const std::string& file_path);
		ShaderFileLoader(std::string&& file_path);

	private:
		void load(Shader* resource) override;
	};
}
