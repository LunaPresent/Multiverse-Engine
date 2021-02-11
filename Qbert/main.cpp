#include "mv/Multiverse.h"
#include "mv/Entity.h"

#include "FPSCounterComponent.h"
#include <iostream>

int main(int argc, char* const argv[])
{
	mv::multiverse().init();
	constexpr mv::id_type a = mv::Component::type_id<FPSCounterComponent>;

	mv::id_type u_id = mv::multiverse().create_universe();
	mv::id_type fps_counter_id = mv::multiverse().create_entity(u_id);
	mv::multiverse().entity(fps_counter_id).add_component<FPSCounterComponent>();

	mv::multiverse().run();

	return 0;
}
