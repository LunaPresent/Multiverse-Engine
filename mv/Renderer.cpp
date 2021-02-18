#include "pch.h"
#include "Renderer.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <glad/glad.h>

#include "SceneManager.h"
#include "ResourceManager.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"


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
		Scene& scene = SceneManager::instance().scene(scene_id);

		for (const Scene::Bucket& bucket : scene) {
			const Material* material = ResourceManager::instance().get<Material>(bucket.material_id);
			const Shader* shader = ResourceManager::instance().get<Shader>(material->shader_id());

			// set material data (shader program, textures...)
			glUseProgram(shader->program());
			for (size_type i = 0; i < material->texture_count(); ++i) {
				glUniform1i(material->sampler_location(i), i);
			}
			for (size_type i = 0; i < material->texture_count(); ++i) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, ResourceManager::instance().get<Texture>(material->texture_id(i))->texture_handle());
			}

			if (material->blend()) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else {
				glDisable(GL_BLEND);
			}

			// set scene data (view/projection...)
			glUniformMatrix4fv(material->view_location(), 1, GL_FALSE, &scene.view()[0][0]);
			glUniformMatrix4fv(material->proj_location(), 1, GL_FALSE, &scene.proj()[0][0]);

			for (id_type object_id : bucket.scene_object_ids) {
				SceneObject& object = SceneManager::instance().scene_object(object_id);

				// set object data (model transform, mesh)
				glUniformMatrix4fv(material->model_location(), 1, GL_FALSE, &object.model_transform()[0][0]);
				const Mesh* mesh = ResourceManager::instance().get<Mesh>(object.mesh_id());
				glBindVertexArray(mesh->vao());
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo());

				// draw call
				glDrawElements(GL_TRIANGLES, mesh->element_count(), GL_UNSIGNED_INT, nullptr);
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
