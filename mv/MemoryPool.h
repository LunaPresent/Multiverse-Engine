#pragma once
#include "setup.h"

#include <type_traits>	// enable_if, is_base_of, has_virtual_destructor
#include <vector>

#include "TemplateUtils.h"

namespace mv
{
	template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
	class MemoryPoolIterator;

	template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment = 64u>
	class MemoryPool final
	{
		static_assert(std::has_virtual_destructor<BaseType>::value, "MemoryPool: BaseType destructor should be virtual");

		friend MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>;
		friend MemoryPoolIterator<const BaseType, block_size, max_obj_size, memory_alignment>;

	public:
		using iterator = MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>;
		using const_iterator = MemoryPoolIterator<const BaseType, block_size, max_obj_size, memory_alignment>;

	private:
		struct MemInfo
		{
			unsigned int prev;
			unsigned int next;

			virtual ~MemInfo() = default; // make polymorphic so BaseType's vtable pointer doesn't get overridden
		};

		static constexpr unsigned int _base_size =
			1u << (log2<(sizeof(BaseType) < sizeof(MemInfo) ? sizeof(MemInfo) : sizeof(BaseType)) - 1u>::value + 1u);
		static constexpr unsigned int _block_size = 1u << (log2<block_size - 1u>::value + 1u);
		static constexpr unsigned int _max_obj_size = 1u << (log2<max_obj_size - 1u>::value + 1u);
		static constexpr unsigned int _chunk_id_bits = log2<log2<_max_obj_size / _base_size>::value>::value + 1u;
		static constexpr unsigned int _memory_alignment = 1u << (log2<memory_alignment - 1u>::value + 1u);
		static constexpr unsigned int _chunk_count = log2<_max_obj_size / _base_size>::value + 1u;

		class Block final
		{
		public:
			struct Header
			{
				MemInfo info; // keep this at the start of the struct no matter so we can just init prev and next with 0
				void* data;
				unsigned int stride;
			};

		private:
			static constexpr unsigned int _id_mask = _block_size - 1u;

			unsigned char* _head;

		public:
			Block(unsigned int stride);
			Block(const Block&) = delete;
			Block(Block&& other) noexcept;

			~Block();

			Block& operator=(const Block&) = delete;
			Block& operator=(Block&& other) noexcept;

			bool full() const;

			template <typename ObjectType, typename... Args>
			unsigned int create(Args&&... args) const;
			void destroy(unsigned int id) const;
			BaseType* get(unsigned int id) const;

		private:
			Header* _header() const;
			unsigned int& _prev(unsigned int id) const;
			unsigned int& _next(unsigned int id) const;
		};

		class Chunk final
		{
		private:
			static constexpr unsigned int _id_mask = ((1u << (32u - _chunk_id_bits)) - 1u) ^ (_block_size - 1u);

			std::vector<Block> _blocks;

		public:
			Chunk() = default;
			Chunk(const Chunk&) = delete;
			Chunk(Chunk&& other) noexcept;

			~Chunk() = default;

			Chunk& operator=(const Chunk&) = delete;
			Chunk& operator=(Chunk&& other) noexcept;

			template <typename ObjectType, typename... Args>
			unsigned int create(Args&&... args);
			void destroy(unsigned int id);
			BaseType* get(unsigned int id) const;

			unsigned int size() const;
		};


		Chunk _chunks[_chunk_count];

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
			unsigned int create(Args&&... args);
		void destroy(unsigned int id);
		BaseType* get(unsigned int id);
		const BaseType* get(unsigned int id) const;

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
	};

	template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
	class MemoryPoolIterator
	{
		friend MemoryPool<typename std::remove_const<BaseType>::type, block_size, max_obj_size, memory_alignment>;

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = BaseType;
		using difference_type = std::ptrdiff_t;
		using pointer = BaseType*;
		using reference = BaseType*;
		using memory_pool_type = typename std::conditional<std::is_const<BaseType>::value,
			const MemoryPool<typename std::remove_const<BaseType>::type, block_size, max_obj_size, memory_alignment>,
			MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>>::type;

	private:
		using byte_ptr = typename std::conditional<std::is_const<BaseType>::value, const unsigned char, unsigned char>::type*;

		memory_pool_type* _memory_pool;
		byte_ptr _current_obj;	// cached pointer to the object, avoiding calling get on the memory pool every time
		unsigned int _current_id;

		static MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment> begin(memory_pool_type& memory_pool);
		static MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment> end(memory_pool_type& memory_pool);

	public:
		MemoryPoolIterator();
		MemoryPoolIterator(const MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>&) = default;

		MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>& operator=(
			const MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>&) = default;

		bool operator==(const MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>& rhs) const;
		bool operator!=(const MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>& rhs) const;

		reference operator*() const;
		pointer operator->() const;

		MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>& operator++();
		MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment> operator++(int);
	};
}

#include "MemoryPool.inl"
