#pragma once
#include "setup.h"

#include <type_traits>	// enable_if, is_base_of, has_virtual_destructor
#include <vector>

#include "TemplateUtils.h"

namespace mv
{
	template <typename BaseType, uint block_size, uint max_obj_size, uint memory_alignment = 64u>
	class MemoryPool final
	{
		static_assert(std::has_virtual_destructor<BaseType>::value, "MemoryPool: BaseType destructor should be virtual");

	private:
		struct MemInfo
		{
			id_type prev;
			id_type next;

			virtual ~MemInfo() = default; // make polymorphic so BaseType's vtable pointer doesn't get overridden
		};

		static constexpr size_type _base_size =
			1u << (log2<(sizeof(BaseType) < sizeof(MemInfo) ? sizeof(MemInfo) : sizeof(BaseType)) - 1u>::value + 1u);
		static constexpr size_type _block_size = 1u << (log2<block_size - 1u>::value + 1u);
		static constexpr size_type _max_obj_size = 1u << (log2<max_obj_size - 1u>::value + 1u);
		static constexpr size_type _obj_size_bits = log2<log2<_max_obj_size / _base_size>::value>::value + 1u;
		static constexpr size_type _memory_alignment = 1u << (log2<memory_alignment - 1u>::value + 1u);

		class Block final
		{
		private:
			static constexpr id_type _id_mask = _block_size - 1u;

			struct Header
			{
				MemInfo info; // keep this at the start of the struct no matter so we can just init prev and next with 0
				void* data;
				size_type object_size;
			};

			byte* _head;

		public:
			Block(size_type object_size);
			Block(const Block&) = delete;
			Block(Block&& other) noexcept;

			~Block();

			Block& operator=(const Block&) = delete;
			Block& operator=(Block&& other) noexcept;

			bool full() const;

			template <typename ObjectType, typename... Args>
			id_type create(Args&&... args) const;
			void destroy(id_type id) const;
			BaseType* get(id_type id) const;

		private:
			Header* _header() const;
			id_type& _prev(id_type id) const;
			id_type& _next(id_type id) const;
		};

		class Chunk final
		{
		private:
			static constexpr id_type _id_mask = ((1u << (32u - _obj_size_bits)) - 1u) ^ (_block_size - 1u);

			std::vector<Block> _blocks;

		public:
			Chunk() = default;
			Chunk(const Chunk&) = delete;
			Chunk(Chunk&& other) noexcept;

			~Chunk() = default;

			Chunk& operator=(const Chunk&) = delete;
			Chunk& operator=(Chunk&& other) noexcept;

			template <typename ObjectType, typename... Args>
			id_type create(Args&&... args);
			void destroy(id_type id);
			BaseType* get(id_type id) const;
		};


		Chunk _chunks[log2<_max_obj_size / _base_size>::value + 1u];

	public:
		MemoryPool() = default;
		MemoryPool(const MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>&) = delete;
		MemoryPool(MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>&& other) noexcept = default;

		~MemoryPool() = default;

		MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>&
			operator=(const MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>&) = delete;
		MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>&
			operator=(MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>&& other) noexcept = default;


		template <typename ObjectType, typename... Args,
			typename std::enable_if<std::is_base_of<BaseType, ObjectType>::value, int>::type = 0>
			id_type create(Args&&... args);
		void destroy(id_type id);
		BaseType* get(id_type id) const;
	};
}

#include "MemoryPool.inl"
