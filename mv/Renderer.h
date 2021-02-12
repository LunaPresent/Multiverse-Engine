#pragma once
#include "setup.h"
#include "Multiverse.h"
#include "Vector.h"

struct SDL_Window;

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
