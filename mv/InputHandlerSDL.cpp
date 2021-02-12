#include "pch.h"
#include "InputHandlerSDL.h"

#include <SDL.h>


bool mv::InputHandlerSDL::update()
{
	bool exit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			exit = true;
		}
	}

	return exit;
}
