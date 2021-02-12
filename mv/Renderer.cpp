#include "pch.h"
#include "Renderer.h"
#include <SDL.h>
#include "SceneManager.h"
#include "Texture2D.h"

void dae::Renderer::Init(SDL_Window * window)
{
	m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_Renderer == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}
}

void dae::Renderer::Render() const
{
	SDL_RenderClear(m_Renderer);

	SceneManager::GetInstance().Render();
	
	SDL_RenderPresent(m_Renderer);
}

void dae::Renderer::Destroy()
{
	if (m_Renderer != nullptr)
	{
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_Rect dst;
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_Rect dst;
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width);
	dst.h = static_cast<int>(height);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}



//#include "pch.h"
//#include "Renderer.h"

//#include <SDL.h>
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

	SDL_GL_SwapWindow(this->_window);
}


void* mv::Renderer::native_window_handle() const
{
	SDL_SysWMinfo wmInfo;
	SDL_GetWindowWMInfo(this->_window, &wmInfo);
	return static_cast<void*>(wmInfo.info.win.window);
}
