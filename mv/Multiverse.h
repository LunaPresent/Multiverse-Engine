#pragma once
#include "setup.h"

#include "IDList.h"

namespace mv
{
	class Entity;
	class Universe;

	class Multiverse
	{
	public:
		static const float tick_interval;
		static const uint tick_frequency;

	private:
		IDList<Entity, id_type> _entities;
		IDList<Universe, id_type> _universes;


		Multiverse();

	public:
		static Multiverse& get();

		void init();
		void cleanup();

		void run();
		//void update();  some sort of "render one frame" alternative to run?


		Entity& entity(id_type id);
		Universe& universe(id_type id);

		id_type create_entity(id_type universe_id);
		id_type create_universe();
	};

	Multiverse& multiverse();
}