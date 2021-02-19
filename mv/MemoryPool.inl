#include "MemoryPool.h"



template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::Block(size_type object_size)
	: _head{ nullptr }
{
	void* data = ::operator new(_block_size + _memory_alignment + sizeof(Header));
	this->_head = reinterpret_cast<byte*>(((reinterpret_cast<std::size_t>(data) + _memory_alignment + sizeof(Header) - 1u)
		/ _memory_alignment) * _memory_alignment - sizeof(Header));
	Header* header = this->_header();
	id_type id;
	for (id = sizeof(Header); id < _block_size; id += object_size) {
		::new(this->_head + id) BaseType();
		this->_prev(id) = id - object_size;
		this->_next(id) = id + object_size;
	}
	id_type last_id = id - object_size;
	this->_prev(sizeof(Header)) = 0;
	this->_next(last_id) = 0;
	header->info.prev = last_id;
	header->info.next = sizeof(Header);
	header->data = data;
	header->object_size = object_size;
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::Block(Block&& other) noexcept
	: _head{ other._head }
{
	other._head = nullptr;
}


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::~Block()
{
	if (this->_head) {
		Header* header = this->_header();
		for (id_type id = sizeof(Header); id < _block_size; id += header->object_size) {
			this->destroy(id);
		}

		::operator delete(header->data);
		this->_head = nullptr;
	}
}


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
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


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline bool mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::full() const
{
	return this->_next(0) == 0;
}


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
template <typename ObjectType, typename... Args>
inline mv::id_type mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::create(Args&&... args) const
{
	id_type id = this->_next(0);
	// remove this entry from linked list by connecting prev and next
	this->_next(0) = this->_next(id);
	this->_prev(this->_next(0)) = 0;
	// call constructor
	::new(this->_head + id) ObjectType(std::forward<Args>(args)...);
	return id;
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::destroy(id_type id) const
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

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::get(id_type id) const
{
	id &= _id_mask;
	return reinterpret_cast<BaseType*>(this->_head + id);
}


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::Header*
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::_header() const
{
	return reinterpret_cast<Header*>(this->_head);
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline mv::id_type& mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::_prev(id_type id) const
{
	return reinterpret_cast<MemInfo*>(this->_head + id)->prev;
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline mv::id_type& mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Block::_next(id_type id) const
{
	return reinterpret_cast<MemInfo*>(this->_head + id)->next;
}




template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::Chunk(Chunk&& other) noexcept
	: _blocks{ std::move(other._blocks) }
{}


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline typename mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk&
mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::operator=(Chunk&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->_blocks = std::move(other._blocks);

	return *this;
}


template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
template <typename ObjectType, typename... Args>
inline mv::id_type mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::create(Args&&... args)
{
	for (uint i = 0; i < this->_blocks.size(); ++i) {
		if (this->_blocks[i].full()) {
			continue;
		}
		id_type id = this->_blocks[i].create<ObjectType>(std::forward<Args>(args)...);
		id += i << log2<_block_size>::value;
		return id;
	}
	uint i = static_cast<uint>(this->_blocks.size());
	constexpr uint obj_size = 1u << (log2<sizeof(ObjectType) - 1u>::value + 1u);
	this->_blocks.push_back(Block(obj_size < _base_size ? _base_size : obj_size));
	id_type id = this->_blocks[i].create<ObjectType>(std::forward<Args>(args)...);
	id += i << log2<_block_size>::value;
	return id;
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::destroy(id_type id)
{
	id_type i = (id & _id_mask) >> log2<_block_size>::value;
	this->_blocks[i].destroy(id);
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::Chunk::get(id_type id) const
{
	id_type i = (id & _id_mask) >> log2<_block_size>::value;
	return this->_blocks[i].get(id);
}




template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
template <typename ObjectType, typename... Args,
	typename std::enable_if<std::is_base_of<BaseType, ObjectType>::value, int>::type>
	inline mv::id_type mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::create(Args&&... args)
{
	uint i = sizeof(ObjectType) < _base_size ? 0 : log2<sizeof(ObjectType) - 1u>::value + 1u - log2<_base_size>::value;
	return this->_chunks[i].create<ObjectType>(std::forward<Args>(args)...) | (i << (32u - _obj_size_bits));
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline void mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::destroy(id_type id)
{
	uint i = id >> (32u - _obj_size_bits);
	this->_chunks[i].destroy(id);
}

template <typename BaseType, mv::uint block_size, mv::uint max_obj_size, mv::uint memory_alignment>
inline BaseType* mv::MemoryPool<BaseType, block_size, max_obj_size, memory_alignment>::get(id_type id) const
{
	uint i = id >> (32u - _obj_size_bits);
	return this->_chunks[i].get(id);
}
