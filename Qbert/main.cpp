#include "mv/Multiverse.h"
#include "mv/Entity.h"

#include "FPSCounterComponent.h"

int main(int argc, char* const argv[])
{
	(void)argc;
	(void)argv;

	mv::multiverse().init();

	mv::id_type u_id = mv::multiverse().create_universe();
	mv::id_type fps_counter_id = mv::multiverse().create_entity(u_id);
	mv::multiverse().entity(fps_counter_id).add_component<FPSCounterComponent>();

	mv::multiverse().run();

	return 0;
}
