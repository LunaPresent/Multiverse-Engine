#pragma once
#include "Singleton.h"

struct SDL_Window;
struct SDL_Renderer;

namespace dae
{
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;

		SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }
	private:
		SDL_Renderer* m_Renderer{};
	};
}


#include "setup.h"
#include "Multiverse.h"
#include "Vector.h"

namespace mv
{
	class Renderer final
	{
		friend Multiverse;

	public:
		struct Settings
		{
			struct {
				std::string title;
				uint16 width;
				uint16 height;
			} window;
			vec3f colour;
		};

	private:
		Settings _settings;
		SDL_Window* _window;
		void* _context;

		Renderer(const Settings& settings);
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;

	public:
		~Renderer();

	private:
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void _init();
		void render() const;

	public:
		void* native_window_handle() const;
	};
}
