#pragma once
#include "Resource.h"

namespace mv
{
	class Mesh final : public Resource
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

	private:
		void _unload() override;
	};

	class MeshLiteralLoader final : public ResourceLoader<Mesh>
	{
	private:
		Mesh::VertexAttribute* _attributes;
		void* _vertices;
		uint* _indices;
		size_type _attribute_count;
		size_type _vertex_buffer_size;
		size_type _index_buffer_size;
		size_type _vertex_size;

	public:
		MeshLiteralLoader(Mesh::VertexAttribute* attributes, size_type attribute_count,
			void* vertices, size_type vertex_count, uint* indices, size_type index_count, bool copy_data = false);

	private:
		void load(Mesh* resource) override;
	};
}
