#pragma once
#include "Resource.h"

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

	public:
		void set_data(uint vao, uint vbo, uint ebo);

		uint vao() const;
		uint vbo() const;
		uint ebo() const;

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

	public:
		MeshTextLoader(id_type font_id/*, text update event to bind to*/);

	private:
		void load(Mesh* resource) override;
	};
}
