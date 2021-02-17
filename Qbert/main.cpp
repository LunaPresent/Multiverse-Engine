#include <vld.h>
#include "mv/Multiverse.h"
#include "mv/Entity.h"

#include "mv/ResourceManager.h"

#include "mv/TextComponent.h"
#include "FPSCounterComponent.h"

int main(int argc, char* const argv[])
{
	(void)argc;
	(void)argv;

	mv::multiverse().init();

	mv::ResourceManager::instance().load("Lingua.otf");
	mv::ResourceManager::instance().load("text_shader.mvs");
	mv::ResourceManager::instance().load("text_material.mvm");

	mv::id_type u_id = mv::multiverse().create_universe();
	mv::id_type fps_counter_id = mv::multiverse().create_entity(u_id);
	mv::multiverse().entity(fps_counter_id).add_component<FPSCounterComponent>();
	mv::multiverse().entity(fps_counter_id).add_component<mv::TextComponent>("Lingua.otf");

	mv::multiverse().run();
	mv::multiverse().cleanup();

	return 0;
}
