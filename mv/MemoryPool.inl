#include "MemoryPool.h"



template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::Block(unsigned int stride)
	: _head{ nullptr }
{
	void* data = ::operator new(_block_size + _memory_alignment + sizeof(Header));
	this->_head = reinterpret_cast<unsigned char*>(
		((reinterpret_cast<std::size_t>(data) + _memory_alignment + sizeof(Header) - 1u)
			/ _memory_alignment) * _memory_alignment - sizeof(Header));
	Header* header = this->_header();
	unsigned int id;
	// only insert items until _block_size instead of _block_size + sizeof(Header) so that id doesn't go out of bounds
	for (id = sizeof(Header); id < _block_size; id += stride) {
		::new(this->_head + id) BaseType();
		this->_prev(id) = id - stride;
		this->_next(id) = id + stride;
	}
	unsigned int last_id = id - stride; // one step back because id is just out of bounds
	this->_prev(sizeof(Header)) = 0;
	this->_next(last_id) = 0;
	header->info.prev = last_id;
	header->info.next = sizeof(Header);
	header->data = data;
	header->stride = stride;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::Block(Block&& other) noexcept
	: _head{ other._head }
{
	other._head = nullptr;
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::~Block()
{
	if (this->_head) {
		Header* header = this->_header();
		for (unsigned int id = sizeof(Header); id < _block_size; id += header->stride) {
			this->destroy(id);
		}

		::operator delete(header->data);
		this->_head = nullptr;
	}
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block&
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::operator=(Block&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	if (this->_head) {
		::operator delete(this->_header()->data);
	}
	this->_head = other._head;
	other._head = nullptr;

	return *this;
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline bool mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::full() const
{
	return this->_next(0) == 0;
}


template<typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline unsigned int mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::reserve() const
{
	unsigned int id = this->_next(0);
	// remove this entry from linked list by connecting prev and next
	this->_next(0) = this->_next(id);
	this->_prev(this->_next(0)) = 0;
	return id;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
template <typename ObjectType, typename... Args>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::create_at(
	unsigned int id, Args&&... args) const
{
	id &= _id_mask;
	::new(this->_head + id) ObjectType(std::forward<Args>(args)...);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::destroy(unsigned int id) const
{
	id &= _id_mask;
	// call destructor and insert new object of basetype
	reinterpret_cast<BaseType*>(this->_head + id)->~BaseType();
	::new(this->_head + id) BaseType();
	// insert this as entry in the linked list
	this->_prev(this->_next(0)) = id;
	this->_next(id) = this->_next(0);
	this->_prev(id) = 0;
	this->_next(0) = id;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::get(unsigned int id) const
{
	id &= _id_mask;
	return reinterpret_cast<BaseType*>(this->_head + id);
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::Header*
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::_header() const
{
	return reinterpret_cast<Header*>(this->_head);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline unsigned int& mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::_prev(unsigned int id) const
{
	return reinterpret_cast<MemInfo*>(this->_head + id)->prev;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline unsigned int& mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::_next(unsigned int id) const
{
	return reinterpret_cast<MemInfo*>(this->_head + id)->next;
}




template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::Chunk(Chunk&& other) noexcept
	: _blocks{ std::move(other._blocks) }
{}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk&
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::operator=(Chunk&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->_blocks = std::move(other._blocks);

	return *this;
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline unsigned int mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::size() const
{
	return static_cast<unsigned int>(this->_blocks.size());
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
template <typename ObjectType>
inline unsigned int mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::reserve()
{
	for (unsigned int i = 0; i < this->_blocks.size(); ++i) {
		if (this->_blocks[i].full()) {
			continue;
		}
		unsigned int id = this->_blocks[i].reserve();
		id += i << log2<_block_size>::value;
		return id;
	}
	unsigned int i = static_cast<unsigned int>(this->_blocks.size());
	constexpr unsigned int stride = 1u << (log2<sizeof(ObjectType) - 1u>::value + 1u);
	this->_blocks.push_back(Block(stride < _base_size ? _base_size : stride));
	unsigned int id = this->_blocks[i].reserve();
	id += i << log2<_block_size>::value;
	return id;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
template <typename ObjectType, typename... Args>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::create_at(
	unsigned int id, Args&&... args) const
{
	unsigned int i = (id & _id_mask) >> log2<_block_size>::value;
	this->_blocks[i].create_at<ObjectType>(id, std::forward<Args>(args)...);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::destroy(unsigned int id)
{
	unsigned int i = (id & _id_mask) >> log2<_block_size>::value;
	this->_blocks[i].destroy(id);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::get(unsigned int id) const
{
	unsigned int i = (id & _id_mask) >> log2<_block_size>::value;
	return this->_blocks[i].get(id);
}




template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
template <typename ObjectType, typename std::enable_if<std::is_base_of<BaseType, ObjectType>::value, int>::type>
inline unsigned int mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::reserve()
{
	unsigned int i = sizeof(ObjectType) < _base_size ? 0 : log2<sizeof(ObjectType) - 1u>::value + 1u - log2<_base_size>::value;
	return this->_chunks[i].reserve<ObjectType>() | (i << (32u - _chunk_id_bits));
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
template <typename ObjectType, typename... Args,
	typename std::enable_if<std::is_base_of<BaseType, ObjectType>::value, int>::type>
	inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::create_at(
		unsigned int id, Args&&... args) const
{
	unsigned int i = id >> (32u - _chunk_id_bits);
	this->_chunks[i].create_at<ObjectType>(id, std::forward<Args>(args)...);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
template <typename ObjectType, typename... Args,
	typename std::enable_if<std::is_base_of<BaseType, ObjectType>::value, int>::type>
	inline unsigned int mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::create(Args&&... args)
{
	unsigned int i = sizeof(ObjectType) < _base_size ? 0 : log2<sizeof(ObjectType) - 1u>::value + 1u - log2<_base_size>::value;
	unsigned int id = this->_chunks[i].reserve<ObjectType>();
	this->_chunks[i].create_at<ObjectType>(id, std::forward<Args>(args)...);
	return id | (i << (32u - _chunk_id_bits));
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::destroy(unsigned int id)
{
	unsigned int i = id >> (32u - _chunk_id_bits);
	this->_chunks[i].destroy(id);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::get(unsigned int id)
{
	unsigned int i = id >> (32u - _chunk_id_bits);
	return this->_chunks[i].get(id);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline const BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::get(unsigned int id) const
{
	unsigned int i = id >> (32u - _chunk_id_bits);
	return this->_chunks[i].get(id);
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::iterator
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::begin()
{
	return iterator::begin(*this);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::iterator
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::end()
{
	return iterator::end(*this);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::const_iterator
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::begin() const
{
	return const_iterator::begin(*this);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::const_iterator
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::end() const
{
	return const_iterator::end(*this);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::const_iterator
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::cbegin() const
{
	return const_iterator::begin(*this);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::const_iterator
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::cend() const
{
	return const_iterator::end(*this);
}




template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>
mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::begin(memory_pool_type& memory_pool)
{
	for (unsigned int i = 0u; i < memory_pool_type::_chunk_count; ++i) {
		if (memory_pool._chunks[i].size() == 0) {
			continue;
		}

		MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment> it;
		it._memory_pool = &memory_pool;
		it._current_id = (i << (32u - memory_pool_type::_chunk_id_bits)) +
			static_cast<unsigned int>(sizeof(typename memory_pool_type::Block::Header));
		it._current_obj = reinterpret_cast<byte_ptr>(memory_pool.get(it._current_id));
		return it;
	}
	return end(memory_pool);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>
mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::end(memory_pool_type& memory_pool)
{
	MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment> it;
	it._memory_pool = &memory_pool;
	it._current_obj = nullptr;
	it._current_id = 0u;
	return it;
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::MemoryPoolIterator()
	: _memory_pool{ nullptr }, _current_obj{ nullptr }, _current_id{ 0u }
{}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline bool mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::operator==(
	const MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>& rhs) const
{
	return this->_current_obj == rhs._current_obj;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline bool mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::operator!=(
	const MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>& rhs) const
{
	return !(*this == rhs);
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::reference
mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::operator*() const
{
	return reinterpret_cast<pointer>(this->_current_obj);
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline typename mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::pointer
mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::operator->() const
{
	return reinterpret_cast<pointer>(this->_current_obj);
}


template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>&
mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::operator++()
{
	constexpr unsigned int chunk_idx_mask = ~((1u << (32u - memory_pool_type::_chunk_id_bits)) - 1u);
	constexpr unsigned int block_idx_mask =
		((1u << (32u - memory_pool_type::_chunk_id_bits)) - 1u) ^ (memory_pool_type::_block_size - 1u);
	unsigned int chunk_idx = this->_current_id >> (32u - memory_pool_type::_chunk_id_bits);
	unsigned int stride = (1u << chunk_idx) * memory_pool_type::_base_size;

	// if next id is in the same block, just increment by stride
	if ((this->_current_id & block_idx_mask) == ((this->_current_id + stride) & block_idx_mask)) {
		this->_current_id += stride;
		this->_current_obj += stride;
		return *this;
	}

	unsigned int block_idx = ((this->_current_id & block_idx_mask) >> log2<memory_pool_type::_block_size>::value) + 1;
	// if next block is in the same chunk, set id and obj to that block's first element
	if (block_idx < this->_memory_pool->_chunks[chunk_idx].size()) {
		this->_current_id = (this->_current_id & chunk_idx_mask) + (block_idx << log2<memory_pool_type::_block_size>::value) +
			static_cast<unsigned int>(sizeof(typename memory_pool_type::Block::Header));
		this->_current_obj = reinterpret_cast<byte_ptr>(this->_memory_pool->get(this->_current_id));
		return *this;
	}

	// end of chunk, go to the next chunk with at least one block
	for (unsigned int i = chunk_idx + 1u; i < memory_pool_type::_chunk_count; ++i) {
		if (this->_memory_pool->_chunks[i].size() == 0) {
			continue;
		}

		this->_current_id = (i << (32u - memory_pool_type::_chunk_id_bits)) +
			static_cast<unsigned int>(sizeof(typename memory_pool_type::Block::Header));
		this->_current_obj = reinterpret_cast<byte_ptr>(this->_memory_pool->get(this->_current_id));
		return *this;
	}
	this->_current_id = 0u;
	this->_current_obj = nullptr;
	return *this;
}

template <typename BaseType, unsigned int block_size, unsigned int max_obj_size, unsigned int memory_alignment>
inline mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>
mv::MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment>::operator++(int)
{
	MemoryPoolIterator<BaseType, block_size, max_obj_size, memory_alignment> it(*this);
	this->operator++();
	return it;
}
