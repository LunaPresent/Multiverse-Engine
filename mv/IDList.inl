#include "IDList.h"

#include <limits>	// numeric_limits
#include <utility>	// move

#undef max


template <typename T, typename S, typename Alloc>
mv::IDList<T, S, Alloc>::IDList(const allocator_type& alloc)
	: _elements(alloc), _lookup(), _freed()
{}

template <typename T, typename S, typename Alloc>
mv::IDList<T, S, Alloc>::IDList(std::initializer_list<value_type> values, const allocator_type& alloc)
	: _elements(values, alloc), _lookup(), _freed()
{
	this->_lookup.reserve(values.size());
	for (size_type i = 0; i < static_cast<size_type>(values.size()); ++i) {
		this->_lookup.push_back(i);
	}
}



template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::iterator mv::IDList<T, S, Alloc>::begin()
{
	return iterator(this->_elements.data());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::iterator mv::IDList<T, S, Alloc>::end()
{
	return iterator(this->_elements.data() + this->_elements.size());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_iterator mv::IDList<T, S, Alloc>::cbegin() const
{
	return const_iterator(this->_elements.data());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_iterator mv::IDList<T, S, Alloc>::begin() const
{
	return this->cbegin();
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_iterator mv::IDList<T, S, Alloc>::cend() const
{
	return const_iterator(this->_elements.data() + this->_elements.size());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_iterator mv::IDList<T, S, Alloc>::end() const
{
	return this->cend();
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::reverse_iterator mv::IDList<T, S, Alloc>::rbegin()
{
	return reverse_iterator(this->end());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::reverse_iterator mv::IDList<T, S, Alloc>::rend()
{
	return reverse_iterator(this->begin());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_reverse_iterator mv::IDList<T, S, Alloc>::crbegin() const
{
	return const_reverse_iterator(this->cend());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_reverse_iterator mv::IDList<T, S, Alloc>::rbegin() const
{
	return this->crbegin();
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_reverse_iterator mv::IDList<T, S, Alloc>::crend() const
{
	return const_reverse_iterator(this->cbegin());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_reverse_iterator mv::IDList<T, S, Alloc>::rend() const
{
	return this->crend();
}



template <typename T, typename S, typename Alloc>
bool mv::IDList<T, S, Alloc>::empty() const
{
	return this->_elements.empty();
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::size() const
{
	return static_cast<size_type>(this->_elements.size());
}

template<typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::capacity() const
{
	return static_cast<size_type>(this->_elements.capacity());
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::max_size()
{
	return std::numeric_limits<size_type>::max() - !std::numeric_limits<size_type>::is_signed;
}


template <typename T, typename S, typename Alloc>
bool mv::IDList<T, S, Alloc>::is_reserved(mv::IDList<T, S, Alloc>::size_type id) const
{
	return id < static_cast<size_type>(this->_lookup.size()) && this->_is_reserved(id);
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::next_id() const
{
	return (this->_freed.empty() ? this->size() : this->_freed.back());
}


template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::reference mv::IDList<T, S, Alloc>::operator[](typename mv::IDList<T, S, Alloc>::size_type id)
{
	return this->_elements[this->_lookup[id]];
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_reference mv::IDList<T, S, Alloc>::operator[](mv::IDList<T, S, Alloc>::size_type id) const
{
	return this->_elements[this->_lookup[id]];
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::reference mv::IDList<T, S, Alloc>::at(mv::IDList<T, S, Alloc>::size_type id)
{
	return this->_elements[this->_lookup[id]];
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_reference mv::IDList<T, S, Alloc>::at(mv::IDList<T, S, Alloc>::size_type id) const
{
	return this->_elements[this->_lookup[id]];
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::pointer mv::IDList<T, S, Alloc>::data() noexcept
{
	return this->_elements.data();
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::const_pointer mv::IDList<T, S, Alloc>::data() const noexcept
{
	return this->_elements.data();
}


template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::insert(const typename mv::IDList<T, S, Alloc>::value_type& element)
{
	if (this->_freed.empty()) {
		this->_lookup.push_back(this->size());
	}
	else {
		this->_lookup[this->_freed.back()] = this->size();
		this->_freed.pop_back();
	}
	this->_elements.push_back(element);
	return this->size() - 1;
}

template<typename T, typename S, typename Alloc>
bool mv::IDList<T, S, Alloc>::insert_at(size_type id, const value_type& element)
{
	if (!this->_request(id)) {
		return false;
	}
	this->_elements.push_back(element);
	return true;
}

template <typename T, typename S, typename Alloc>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::insert(typename mv::IDList<T, S, Alloc>::value_type&& element)
{
	if (this->_freed.empty()) {
		this->_lookup.push_back(this->size());
	}
	else {
		this->_lookup[this->_freed.back()] = this->size();
		this->_freed.pop_back();
	}
	this->_elements.push_back(std::move(element));
	return this->size() - 1;
}

template <typename T, typename S, typename Alloc>
bool mv::IDList<T, S, Alloc>::insert_at(size_type id, value_type&& element)
{
	if (!this->_request(id)) {
		return false;
	}
	this->_elements.push_back(std::move(element));
	return true;
}

template <typename T, typename S, typename Alloc>
template <typename... Args>
typename mv::IDList<T, S, Alloc>::size_type mv::IDList<T, S, Alloc>::emplace(Args&&... args)
{
	if (this->_freed.empty()) {
		this->_lookup.push_back(this->size());
	}
	else {
		this->_lookup[this->_freed.back()] = this->size();
		this->_freed.pop_back();
	}
	this->_elements.emplace_back(std::forward<Args>(args)...);
	return this->size() - 1;
}

template <typename T, typename S, typename Alloc>
template <typename... Args>
bool mv::IDList<T, S, Alloc>::emplace_at(size_type id, Args&&... args)
{
	if (!this->_request(id)) {
		return false;
	}
	this->_elements.emplace_back(std::forward<Args>(args)...);
	return true;
}

template <typename T, typename S, typename Alloc>
void mv::IDList<T, S, Alloc>::erase(typename mv::IDList<T, S, Alloc>::size_type id)
{
	this->at(id) = std::move(this->_elements.back());
	this->_elements.pop_back();
	this->_lookup[this->size()] = this->_lookup[id];
	this->_lookup[id] = static_cast<size_type>(-1);
	this->_freed.push_back(id);
}

template <typename T, typename S, typename Alloc>
void mv::IDList<T, S, Alloc>::clear()
{
	this->_elements.clear();
	this->_lookup.clear();
	this->_freed.clear();
}

template <typename T, typename S, typename Alloc>
void mv::IDList<T, S, Alloc>::shrink_to_fit()
{
	this->_elements.shrink_to_fit();
	this->_lookup.shrink_to_fit();
	this->_freed.shrink_to_fit();
}



template <typename T, typename S, typename Alloc>
bool mv::IDList<T, S, Alloc>::_is_reserved(size_type id) const
{
	return this->_lookup[id] != static_cast<size_type>(-1);
}


template <typename T, typename S, typename Alloc>
bool mv::IDList<T, S, Alloc>::_request(size_type id)
{
	if (id >= static_cast<size_type>(this->_lookup.size())) {
		for (size_type i = static_cast<size_type>(this->_lookup.size()); i < id; ++i) {
			this->_freed.push_back(i);
		}
		this->_lookup.resize(id + 1, static_cast<size_type>(-1));
	}
	else if (!this->_is_reserved(id)) {
		auto i = std::find(this->_freed.begin(), this->_freed.end(), id) - this->_freed.begin();
		this->_freed[i] = this->_freed.back();
		this->_freed.pop_back();
	}
	else {
		return false;
	}
	this->_lookup[id] = this->size();
	return true;
}




template <typename T, typename S, typename Alloc>
mv::IDListIterator<T, S, Alloc>::IDListIterator(typename IDListIterator<T, S, Alloc>::pointer ptr)
	: _ptr{ ptr }
{}

template <typename T, typename S, typename Alloc>
template <typename> // cast to const iterator
mv::IDListIterator<T, S, Alloc>::IDListIterator(const typename mv::IDListIterator<T, S, Alloc>::iterator& obj)
	: _ptr{ obj._ptr }
{}



template <typename T, typename S, typename Alloc>
bool mv::IDListIterator<T, S, Alloc>::operator==(const typename mv::IDListIterator<T, S, Alloc>::iterator& rhs) const
{
	return this->_ptr == rhs._ptr;
}

template <typename T, typename S, typename Alloc>
bool mv::IDListIterator<T, S, Alloc>::operator==(const typename mv::IDListIterator<T, S, Alloc>::const_iterator& rhs) const
{
	return this->_ptr == rhs._ptr;
}

template <typename T, typename S, typename Alloc>
bool mv::IDListIterator<T, S, Alloc>::operator!=(const typename mv::IDListIterator<T, S, Alloc>::iterator& rhs) const
{
	return !(*this == rhs);
}

template <typename T, typename S, typename Alloc>
bool mv::IDListIterator<T, S, Alloc>::operator!=(const typename mv::IDListIterator<T, S, Alloc>::const_iterator& rhs) const
{
	return !(*this == rhs);
}


template <typename T, typename S, typename Alloc>
typename mv::IDListIterator<T, S, Alloc>::reference mv::IDListIterator<T, S, Alloc>::operator*() const
{
	return *this->_ptr;
}

template <typename T, typename S, typename Alloc>
typename mv::IDListIterator<T, S, Alloc>::pointer mv::IDListIterator<T, S, Alloc>::operator->() const
{
	return this->_ptr;
}


template <typename T, typename S, typename Alloc>
mv::IDListIterator<T, S, Alloc>& mv::IDListIterator<T, S, Alloc>::operator++()
{
	++this->_ptr;
	return *this;
}

template <typename T, typename S, typename Alloc>
mv::IDListIterator<T, S, Alloc> mv::IDListIterator<T, S, Alloc>::operator++(int)
{
	mv::IDListIterator<T> retval(*this);
	++(*this);
	return retval;
}


template <typename T, typename S, typename Alloc>
mv::IDListIterator<T, S, Alloc>& mv::IDListIterator<T, S, Alloc>::operator--()
{
	--this->_ptr;
	return *this;
}

template <typename T, typename S, typename Alloc>
mv::IDListIterator<T, S, Alloc> mv::IDListIterator<T, S, Alloc>::operator--(int)
{
	mv::IDListIterator<T, S, Alloc> retval(*this);
	--(*this);
	return retval;
}