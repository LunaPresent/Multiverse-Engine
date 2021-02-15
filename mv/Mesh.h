#pragma once
#include "Resource.h"

#include "Event.h"

namespace mv
{
	class Mesh : public Resource
	{
	public:
		struct VertexAttribute
		{
			size_type component_count;
		};

	private:
		uint _vao;
		uint _vbo;
		uint _ebo;
		uint _element_count;

	public:
		void set_data(uint vao, uint vbo, uint ebo, uint _element_count);

		uint vao() const;
		uint vbo() const;
		uint ebo() const;
		uint element_count() const;

	protected:
		void _unload() override;
	};

	class MeshLiteralLoader final : public ResourceLoader<Mesh>
	{
	private:
		Mesh::VertexAttribute* _attributes;
		void* _vertices;
		uint* _indices;
		size_type _attribute_count;
		size_type _vertex_count;
		size_type _index_count;
		size_type _vertex_size;

	public:
		MeshLiteralLoader(Mesh::VertexAttribute* attributes, size_type attribute_count,
			void* vertices, size_type vertex_count, uint* indices, size_type index_count, bool copy_data = false);
		~MeshLiteralLoader();

	private:
		void load(Mesh* resource) override;
	};

	class MeshTextLoader final : public ResourceLoader<Mesh>
	{
		id_type _font_id;
		EventRef<std::string> _text_update_event;
		std::string _text;
		Mesh* _mesh;

	public:
		MeshTextLoader(id_type font_id, const EventRef<std::string>& text_update_event);

	private:
		void load(Mesh* resource) override;

		void update_text(const std::string& text);
		void update_mesh();
	};
}
