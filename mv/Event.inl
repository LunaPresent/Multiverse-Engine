#include "Event.h"

#include <utility>	// forward


template <typename... EventArgs>
mv::Event<EventArgs...>::Event()
	: _data{ new EventData }
{}

template <typename... EventArgs>
mv::Event<EventArgs...>::Event(Event<EventArgs...>&& other) noexcept
	: _data{ other._data }
{
	other._data = nullptr;
}


template <typename... EventArgs>
mv::Event<EventArgs...>::~Event()
{
	if (this->_data) {
		for (EventRef<EventArgs...>* ref : this->_data->refs) {
			ref->_data = nullptr;
		}
		delete this->_data;
		this->_data = nullptr;
	}
}


template <typename... EventArgs>
mv::Event<EventArgs...>& mv::Event<EventArgs...>::operator=(Event<EventArgs...>&& other) noexcept
{
	if (this == &other) {
		return *this;
	}
	this->_data = other._data;
	other._data = nullptr;
	return *this;
}


template <typename... EventArgs>
mv::EventRef<EventArgs...> mv::Event<EventArgs...>::ref() const
{
	return EventRef<EventArgs...>(this->_data);
}


template <typename... EventArgs>
template <typename F, typename>
mv::EventRef<EventArgs...> mv::Event<EventArgs...>::ref(F&& callback) const
{
	return EventRef<EventArgs...>(this->_data, this->_data.callbacks.insert(std::forward<F>(callback)));
}


template <typename... EventArgs>
void mv::Event<EventArgs...>::clear()
{
	for (EventRef<EventArgs...>* ref : this->_data.refs) {
		ref->_callback_id = invalid_id;
	}
	this->_data.callbacks.clear();
}


template <typename... EventArgs>
template <typename... Args, typename>
void mv::Event<EventArgs...>::raise(Args&&... e)
{
	for (const callback_type& callback : this->_data->callbacks) {
		callback(e...);
	}
}




template <typename... EventArgs>
mv::EventRef<EventArgs...>::EventRef()
	: _data{ nullptr }, _callback_id{ invalid_id }
{}

template <typename... EventArgs>
mv::EventRef<EventArgs...>::EventRef(typename Event<EventArgs...>::EventData* data)
	: _data{ data }, _callback_id{ invalid_id }
{
	this->_data->refs.insert(this);
}

template <typename... EventArgs>
mv::EventRef<EventArgs...>::EventRef(typename Event<EventArgs...>::EventData* data, id_type callback_id)
	: _data{ data }, _callback_id{ callback_id }
{
	this->_data->refs.insert(this);
}

template <typename... EventArgs>
mv::EventRef<EventArgs...>::EventRef(EventRef<EventArgs...>&& other) noexcept
	: _data{ other._data }, _callback_id{ other._callback_id }
{
	if (this->_data) {
		this->_data->refs.erase(other);
		this->_data->refs.insert(this);
		other._data = nullptr;
	}
}


template <typename... EventArgs>
mv::EventRef<EventArgs...>::~EventRef()
{
	if (!this->_data)
		return;
	if (this->_callback_id != invalid_id) {
		this->_data->callbacks.erase(this->_callback_id);
	}
	this->_data->refs.erase(this);
	this->_data = nullptr;
}


template <typename... EventArgs>
mv::EventRef<EventArgs...>& mv::EventRef<EventArgs...>::operator=(EventRef<EventArgs...>&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	if (this->_data) {
		if (this->_callback_id != invalid_id) {
			this->_data->callbacks.erase(this->_callback_id);
		}
		this->_data->refs.erase(this);
	}

	this->_data = other._data;
	this->_callback_id = other._callback_id;
	if (this->_data) {
		this->_data->refs.erase(&other);
		this->_data->refs.insert(this);
		other._data = nullptr;
	}

	return *this;
}


template <typename... EventArgs>
mv::EventRef<EventArgs...> mv::EventRef<EventArgs...>::ref() const
{
	return EventRef<EventArgs...>(this->_data);
}

template <typename... EventArgs>
template <typename F, typename>
mv::EventRef<EventArgs...> mv::EventRef<EventArgs...>::ref(F&& callback) const
{
	return EventRef<EventArgs...>(this->_data, this->_data->callbacks.insert(std::forward<F>(callback)));
}


template <typename... EventArgs>
bool mv::EventRef<EventArgs...>::valid() const
{
	return this->_data;
}

template <typename... EventArgs>
bool mv::EventRef<EventArgs...>::has_callback() const
{
	return this->_data && this->_callback_id != invalid_id;
}
