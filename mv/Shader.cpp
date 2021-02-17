#include "pch.h"
#include "Shader.h"

#include <fstream>	// ifstream
#include <utility>	// move

#include <glad/glad.h>
#include <json.h>

#include "ResourceManager.h"



void mv::Shader::set_data(uint program_handle, std::map<std::string, int>&& uniform_locations)
{
	this->_program_handle = program_handle;
	this->_uniform_locations = std::move(uniform_locations);
}


mv::uint mv::Shader::program() const
{
	return this->_program_handle;
}

int mv::Shader::uniform_location(const std::string& uniform_name) const
{
	auto it = this->_uniform_locations.find(uniform_name);
	return (it != this->_uniform_locations.end() ? it->second : -1);
}


void mv::Shader::_unload()
{
	glDeleteProgram(this->_program_handle);
	this->_program_handle = 0;
}




mv::ShaderFileLoader::ShaderFileLoader(const std::string& file_path)
	: _file_path(file_path)
{}

mv::ShaderFileLoader::ShaderFileLoader(std::string&& file_path)
	: _file_path(std::move(file_path))
{}


void mv::ShaderFileLoader::load(Shader* resource)
{
	std::ifstream f;

	f.open(this->_file_path);
	nlohmann::json j;
	f >> j;
	f.close();

	std::string vertex_shader_path = ResourceManager::instance().data_path() + j.find("vert")->get<std::string>();
	std::string fragment_shader_path = ResourceManager::instance().data_path() + j.find("frag")->get<std::string>();

	uint file_size;
	// shader loading from https://learnopengl.com/Getting-started/Shaders
	int success;
	char infoLog[512];

	// vertex Shader
	f.open(vertex_shader_path, std::ios::binary);
	f.seekg(0, std::ios::end);
	file_size = static_cast<uint>(f.tellg());
	char* vertex_source = new char[file_size + 1];
	f.seekg(0, std::ios::beg);
	f.read(vertex_source, file_size);
	f.close();
	vertex_source[file_size] = '\0';

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);
	// print compile errors if any
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	delete[] vertex_source;

	// fragment Shader
	f.open(fragment_shader_path, std::ios::binary);
	f.seekg(0, std::ios::end);
	file_size = static_cast<uint>(f.tellg());
	char* fragment_source = new char[file_size + 1];
	f.seekg(0, std::ios::beg);
	f.read(fragment_source, file_size);
	f.close();
	fragment_source[file_size] = '\0';

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);
	// print compile errors if any
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	delete[] fragment_source;

	// shader Program
	uint program_handle = glCreateProgram();
	glAttachShader(program_handle, vertex_shader);
	glAttachShader(program_handle, fragment_shader);
	glLinkProgram(program_handle);
	// print linking errors if any
	glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program_handle, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// register all uniforms
	std::map<std::string, int> uniform_locations;
	GLint name_buffer_size;
	GLint uniform_count;
	glGetProgramiv(program_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_buffer_size);
	glGetProgramiv(program_handle, GL_ACTIVE_UNIFORMS, &uniform_count);
	char* name = new char[name_buffer_size];
	for (GLint i = 0; i < uniform_count; ++i) {
		GLint size;
		GLenum type;
		glGetActiveUniform(program_handle, i, name_buffer_size, nullptr, &size, &type, name);
		uniform_locations.emplace(name, glGetUniformLocation(program_handle, name));
	}
	delete[] name;

	resource->set_data(program_handle, std::move(uniform_locations));
}
