#include "pch.h"
#include "Mesh.h"

#include <algorithm>    // copy

#include <glad/glad.h>


void mv::Mesh::set_data(uint vao, uint vbo, uint ebo)
{
	this->_vao = vao;
	this->_vbo = vbo;
	this->_ebo = ebo;
}


mv::uint mv::Mesh::vao() const
{
	return this->_vao;
}


void mv::Mesh::_unload()
{
	glDeleteVertexArrays(1, &this->_vao);
	glDeleteBuffers(1, &this->_vbo);
	glDeleteBuffers(1, &this->_ebo);
}




mv::MeshLiteralLoader::MeshLiteralLoader(Mesh::VertexAttribute* attributes, size_type attribute_count,
	void* vertices, size_type vertex_count, uint* indices, size_type index_count, bool copy_data)
	: _attributes{ copy_data ? new Mesh::VertexAttribute[attribute_count] : attributes }, _vertices{ copy_data ? nullptr : vertices },
	_indices{ copy_data ? new uint[index_count] : indices }, _attribute_count{ attribute_count },
	_vertex_buffer_size{}, _index_buffer_size{ index_count * sizeof(*indices) }, _vertex_size{ 0 }
{
	for (size_type i = 0; i < attribute_count; ++i) {
		this->_vertex_size += attributes[i].component_count * sizeof(float);
	}
	this->_vertex_buffer_size = vertex_count * this->_vertex_size;
	if (copy_data) {
		std::copy(attributes, attributes + attribute_count, this->_attributes);
		std::copy(static_cast<byte*>(vertices), static_cast<byte*>(vertices) + this->_vertex_buffer_size,
			static_cast<byte*>(this->_vertices));
		std::copy(indices, indices + index_count, this->_indices);
	}
}


void mv::MeshLiteralLoader::load(Mesh* resource)
{
	uint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, this->_vertex_buffer_size, this->_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer_size, this->_indices, GL_STATIC_DRAW);

	size_type offset = 0;
	for (size_type i = 0; i < this->_attribute_count; ++i) {
		glVertexAttribPointer(i, this->_attributes[i].component_count,
			GL_FLOAT, GL_FALSE, this->_vertex_size, reinterpret_cast<void*>(offset));
		glEnableVertexAttribArray(i);
		offset += this->_attributes[i].component_count * sizeof(float);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	resource->set_data(vao, vbo, ebo);
}
