#pragma once
#include "setup.h"

#include <functional>	// function
#include <utility>	// declval
#include <set>	// set

#include "IDList.h"

namespace mv
{
	template <typename... EventArgs>
	class EventRef;

	template <typename... EventArgs>
	class Event
	{
	public:
		using callback_type = std::function<void(EventArgs...)>;

		struct EventData
		{
			IDList<callback_type> callbacks;
			std::set<EventRef<EventArgs...>*> refs;
		};

	private:
		EventData* _data;

	public:
		Event();
		Event(Event<EventArgs...>&& other) noexcept;

		~Event();

		Event<EventArgs...>& operator=(Event<EventArgs...>&& other) noexcept;

		/**
			\brief Get blank reference
			\returns a non owning object referencing this event
		*/
		EventRef<EventArgs...> ref() const;
		/**
			\brief Get reference
			\param callback a callable object
			\returns a non owning object referencing this event

			This object "owns" one callback, automatically managing its deletion
		*/
		template <typename F, typename = decltype(callback_type(std::declval<F>()))>
		EventRef<EventArgs...> ref(F&& callback) const;

		
		/**
			\brief Detach all callbacks
		*/
		void clear();

		/**
			\brief Invoke callback functions
		*/
		template <typename... Args, typename = decltype(std::declval<callback_type>()(std::declval<Args>()...))>
		void raise(Args&&... e);
	};

	template <typename... EventArgs>
	class EventRef
	{
		friend Event<EventArgs...>;

	private:
		typename Event<EventArgs...>::EventData* _data;
		id_type _callback_id;

	public:
		EventRef();
	private:
		EventRef(typename Event<EventArgs...>::EventData* data);
		EventRef(typename Event<EventArgs...>::EventData* data, id_type callback_id);
	public:
		EventRef(EventRef<EventArgs...>&& other) noexcept;

		~EventRef();

		EventRef<EventArgs...>& operator=(EventRef<EventArgs...>&& other) noexcept;

		/**
			\brief Get blank reference
			\returns a non owning object referencing the same event as this reference
		*/
		EventRef<EventArgs...> ref() const;
		/**
			\brief Get reference
			\param callback a callable object
			\returns a non owning object referencing the same event as this reference

			This object "owns" one callback, automatically managing its deletion
		*/
		template <typename F, typename = decltype(Event<EventArgs...>::callback_type(std::declval<F>()))>
		EventRef<EventArgs...> ref(F&& callback) const;

		bool valid() const;
		bool has_callback() const;
	};
}

#include "Event.inl"
