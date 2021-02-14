#include "pch.h"
#include "Renderer.h"
#include "SceneManager.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <glad/glad.h>

mv::Renderer::Renderer(const mv::Renderer::Settings& settings)
	: _settings(settings), _window{}, _context{}
{
	this->_init();
}

mv::Renderer::~Renderer()
{
	SDL_GL_DeleteContext(this->_context);
	SDL_DestroyWindow(this->_window);
}


void mv::Renderer::_init()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	this->_window = SDL_CreateWindow(this->_settings.window.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		this->_settings.window.width, this->_settings.window.height, SDL_WINDOW_OPENGL);
	this->_context = SDL_GL_CreateContext(this->_window);

	if (SDL_GL_SetSwapInterval(-1) == -1) { // try to set adaptive vsync
		SDL_GL_SetSwapInterval(1);	// if fail fall back to regular vsync
	}

	if (!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glClearColor(this->_settings.colour.r, this->_settings.colour.g, this->_settings.colour.b, 1);
}


void mv::Renderer::render() const
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (id_type scene_id : SceneManager::instance().active_scene_ids()) {
		for (const Scene::Bucket& bucket : SceneManager::instance().scene(scene_id)) {
			// set material data (shader program, textures...)
			// set scene data (view/projection...)
			for (id_type object_id : bucket.scene_object_ids) {
				// set object data (model transform, mesh)
				SceneManager::instance().scene_object(object_id).model_transform();
				// draw call
			}
		}
	}

	SDL_GL_SwapWindow(this->_window);
}


void* mv::Renderer::native_window_handle() const
{
	SDL_SysWMinfo wmInfo;
	SDL_GetWindowWMInfo(this->_window, &wmInfo);
	return static_cast<void*>(wmInfo.info.win.window);
}
