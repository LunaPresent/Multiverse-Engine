#include "pch.h"
#include "Multiverse.h"

#include <chrono>
#include <type_traits>

#include "Component.h"
#include "Entity.h"
#include "Universe.h"

#include "Renderer.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "integration/FreeType.h"

#include "Input.h"
#include "InputHandlerSDL.h"



#ifndef MV_TICKFREQUENCY
#define MV_TICKFREQUENCY 60
#endif // !MG_TICKFREQUENCY
static_assert(std::is_integral<decltype(MV_TICKFREQUENCY)>::value&& MV_TICKFREQUENCY > 0,
	"[mv] MV_TICKFREQUENCY must be a positive integral value");
static_assert(MV_TICKFREQUENCY <= 10000, "[mv] Tick frequency cannot exceed 10000 Hz");


const float mv::Multiverse::tick_interval = static_cast<float>(1'000'000'000 / MV_TICKFREQUENCY) / 1'000'000'000;
const mv::uint mv::Multiverse::tick_frequency = MV_TICKFREQUENCY;

const mv::ServiceProxy<mv::InputService> mv::input(mv::Multiverse::instance().service_locator().get<mv::InputService>());


mv::Multiverse::Multiverse()
	: _entities(), _universes(), _renderer{}, _service_locator()
{}

mv::Multiverse& mv::Multiverse::instance()
{
	static Multiverse multiverse;
	return multiverse;
}


void mv::Multiverse::init()
{
	Renderer::Settings renderer_settings;
	renderer_settings.colour = { 0.f, 0.125f, 0.25f };
	renderer_settings.window.title = "Prog4 Engine - Luna Present";
	renderer_settings.window.width = 720;
	renderer_settings.window.height = 720;
	this->_renderer = new Renderer(renderer_settings);
	this->_service_locator.set<InputService, InputHandlerSDL>();
	new SceneManager();
	new ResourceManager("../Data");
	new FreeType();
}

void mv::Multiverse::cleanup()
{
	this->_entities.clear();
	this->_universes.clear();
	delete SceneManager::ptr();
	delete ResourceManager::ptr();
	delete FreeType::ptr();
	delete this->_renderer;
	Component::cleanup();
}


void mv::Multiverse::run()
{
	constexpr std::chrono::high_resolution_clock::duration tick_duration(
		std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
			std::chrono::duration<long long, std::nano>(1'000'000'000 / tick_frequency)));

	std::chrono::high_resolution_clock::time_point prev_time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration behind_time(0);
	bool exit = false;
	while (!exit) {
		std::chrono::high_resolution_clock::time_point curr_time = std::chrono::high_resolution_clock::now();
		std::chrono::high_resolution_clock::duration elapsed_time = curr_time - prev_time; // add time since previous loop
		float frame_interval = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed_time).count();
		behind_time += elapsed_time;
		prev_time = curr_time;

		for (Universe& universe : this->_universes) {
			universe.frame_start();
		}
		exit = this->_service_locator.get<InputService>()->update() || exit;
		while (behind_time > tick_duration) {
			for (Universe& universe : this->_universes) {
				universe.fixed_update(tick_interval);
			}
			behind_time -= tick_duration;
		}

		for (Universe& universe : this->_universes) {
			universe.update(frame_interval);
		}
		this->_renderer->render();
	}
}


mv::Entity& mv::Multiverse::entity(id_type id)
{
	return this->_entities[id];
}

mv::Universe& mv::Multiverse::universe(id_type id)
{
	return this->_universes[id];
}


mv::id_type mv::Multiverse::create_entity(id_type universe_id)
{
	return this->_entities.insert(Entity(this->_entities.next_id(), universe_id));
}

mv::id_type mv::Multiverse::create_universe()
{
	return this->_universes.insert(Universe(this->_universes.next_id()));
}




mv::Multiverse& mv::multiverse()
{
	return Multiverse::instance();
}
