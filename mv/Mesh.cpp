#include "pch.h"
#include "Mesh.h"

#include <algorithm>    // copy

#include <glad/glad.h>

#include "ResourceManager.h"
#include "Font.h"


void mv::Mesh::set_data(uint vao, uint vbo, uint ebo, uint element_count)
{
	this->_vao = vao;
	this->_vbo = vbo;
	this->_ebo = ebo;
	this->_element_count = element_count;
}


mv::uint mv::Mesh::vao() const
{
	return this->_vao;
}

mv::uint mv::Mesh::vbo() const
{
	return this->_vbo;
}

mv::uint mv::Mesh::ebo() const
{
	return this->_ebo;
}

mv::uint mv::Mesh::element_count() const
{
	return this->_element_count;
}


void mv::Mesh::_unload()
{
	glDeleteVertexArrays(1, &this->_vao);
	glDeleteBuffers(1, &this->_vbo);
	glDeleteBuffers(1, &this->_ebo);
	this->_vao = 0;
	this->_vbo = 0;
	this->_ebo = 0;
}




mv::MeshLiteralLoader::MeshLiteralLoader(Mesh::VertexAttribute* attributes, size_type attribute_count,
	void* vertices, size_type vertex_count, uint* indices, size_type index_count, bool copy_data)
	: _attributes{ copy_data ? new Mesh::VertexAttribute[attribute_count] : attributes }, _vertices{ copy_data ? nullptr : vertices },
	_indices{ copy_data ? new uint[index_count] : indices }, _attribute_count{ attribute_count },
	_vertex_count{ vertex_count }, _index_count{ index_count }, _vertex_size{ 0 }
{
	for (size_type i = 0; i < attribute_count; ++i) {
		this->_vertex_size += attributes[i].component_count * sizeof(float);
	}
	if (copy_data) {
		std::copy(attributes, attributes + attribute_count, this->_attributes);
		std::copy(static_cast<byte*>(vertices), static_cast<byte*>(vertices) + vertex_count * this->_vertex_size,
			static_cast<byte*>(this->_vertices));
		std::copy(indices, indices + index_count, this->_indices);
	}
}

mv::MeshLiteralLoader::~MeshLiteralLoader()
{
	delete[] this->_attributes;
	delete[] this->_vertices;
	delete[] this->_indices;
	this->_attributes = nullptr;
	this->_vertices = nullptr;
	this->_indices = nullptr;
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

	glBufferData(GL_ARRAY_BUFFER, this->_vertex_count * this->_vertex_size, this->_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_index_count * sizeof(uint), this->_indices, GL_STATIC_DRAW);

	std::size_t offset = 0;
	for (size_type i = 0; i < this->_attribute_count; ++i) {
		glVertexAttribPointer(i, this->_attributes[i].component_count,
			GL_FLOAT, GL_FALSE, this->_vertex_size, reinterpret_cast<void*>(offset));
		glEnableVertexAttribArray(i);
		offset += this->_attributes[i].component_count * sizeof(float);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	resource->set_data(vao, vbo, ebo, this->_index_count);
}




mv::MeshTextLoader::MeshTextLoader(id_type font_id, const EventRef<std::string>& text_update_event)
	: _font_id{ font_id }, _text_update_event(), _text(), _mesh{ nullptr }
{
	this->_text_update_event = text_update_event.ref([this](const std::string& str) { this->update_text(str); });
}


void mv::MeshTextLoader::load(Mesh* resource)
{
	this->_mesh = resource;

	uint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	resource->set_data(vao, vbo, ebo, 0);
}


void mv::MeshTextLoader::update_text(const std::string& text)
{
	this->_text = text;

	if (this->_mesh && this->_mesh->load_state() == Resource::LoadState::loaded) {
		this->update_mesh();
	}
}

void mv::MeshTextLoader::update_mesh()
{
	struct Vertex
	{
		vec2f pos;
		vec2f uv;
	};

	const Font* font = ResourceManager::instance().get<Font>(this->_font_id);

	std::vector<Vertex> vertices;
	vertices.reserve(4 * this->_text.length()); // 4 vertices/glyph
	std::vector<uint> indices;
	indices.reserve(6 * this->_text.length());	// 6 indices/quad, should switch to actually drawing quads instead of elements

	float origin = 0.f;
	float w = static_cast<float>(font->width());
	float h = static_cast<float>(font->height());
	for (uint i = 0; i < this->_text.size(); ++i) {
		char c = this->_text[i];

		vec2f tex_origin{ static_cast<float>(font->width() / Font::chars_per_row * (c % Font::chars_per_row)),
			static_cast<float>(font->height() * Font::chars_per_row / Font::char_count * (c / Font::chars_per_row)) };
		const Font::GlyphData& g = font->glyph_data(c);
		vertices.push_back(Vertex{
			{ origin + static_cast<float>(g.offset.x), static_cast<float>(g.offset.y) },
			{ tex_origin.x / w, tex_origin.y / h } });
		vertices.push_back(Vertex{
			{ origin + static_cast<float>(g.offset.x), static_cast<float>(g.offset.y - g.size.y) },
			{ tex_origin.x / w, (tex_origin.y + static_cast<float>(g.size.y)) / h } });
		vertices.push_back(Vertex{
			{ origin + static_cast<float>(g.offset.x + g.size.x), static_cast<float>(g.offset.y - g.size.y) },
			{ (tex_origin.x + static_cast<float>(g.size.x)) / w, (tex_origin.y + static_cast<float>(g.size.y)) / h } });
		vertices.push_back(Vertex{
			{ origin + static_cast<float>(g.offset.x + g.size.x), static_cast<float>(g.offset.y) },
			{ (tex_origin.x + static_cast<float>(g.size.x)) / w, tex_origin.y / h } });

		indices.push_back(4 * i);
		indices.push_back(4 * i + 1);
		indices.push_back(4 * i + 2);
		indices.push_back(4 * i + 2);
		indices.push_back(4 * i + 3);
		indices.push_back(4 * i);

		origin += g.advance;
	}

	glBindVertexArray(this->_mesh->vao());
	glBindBuffer(GL_ARRAY_BUFFER, this->_mesh->vbo());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_mesh->ebo());

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(vec2f)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->_mesh->set_data(this->_mesh->vao(), this->_mesh->vbo(), this->_mesh->ebo(), static_cast<uint>(indices.size()));
}
