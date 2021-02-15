#pragma once
#include "Texture.h"

#include <vector>

#include "Vector.h"

namespace mv
{
	class Font : public Texture
	{
	public:
		struct GlyphData
		{
			vec2i size;	// size of the glyph in pixels
			vec2i offset;	// offset from the origin to the top left pixel of the glyph
			float advance;	// amount of pixels from origin to the next glyph's origin
		};

		static const uint char_count;
		static const uint chars_per_row;

	private:
		static size_type _instance_count;
		static void* _freetype_lib;

		std::vector<GlyphData> _glyphs;

	public:
		static void* freetype_lib();

		Font();
		~Font();

		void set_data(uint texture_handle, uint width, uint height, std::vector<GlyphData>&& glyphs);

		const GlyphData& glyph_data(char glyph) const;

	protected:
		void _unload() override;
	};

	class FontFileLoader : public ResourceLoader<Font>
	{
	private:
		std::string _file_path;
		uint _font_size;

	public:
		FontFileLoader(const std::string& file_path, uint font_size = 48);
		FontFileLoader(std::string&& file_path, uint font_size = 48);

	private:
		void load(Font* resource) override;
	};
}
