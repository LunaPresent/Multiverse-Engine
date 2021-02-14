#pragma once
#include "setup.h"

#include "IDList.h"
#include "ServiceLocator.h"

namespace mv
{
	class Entity;
	class Universe;

	class Renderer;

	class InputService;

	class Multiverse final
	{
	public:
		using service_locator_type = ServiceLocator<InputService>;

		static const float tick_interval;
		static const uint tick_frequency;

	private:
		IDList<Entity, id_type> _entities;
		IDList<Universe, id_type> _universes;

		Renderer* _renderer;

		service_locator_type _service_locator;


		Multiverse();

	public:
		static Multiverse& instance();

		void init();
		void cleanup();

		void run();
		//void update();  some sort of "render one frame" alternative to run?

		inline const service_locator_type& service_locator() {
			return this->_service_locator;
		}


		Entity& entity(id_type id);
		Universe& universe(id_type id);

		id_type create_entity(id_type universe_id);
		id_type create_universe();
	};

	Multiverse& multiverse();
}