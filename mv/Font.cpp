#include "pch.h"
#include "Font.h"

#include <algorithm>	// copy

#include <glad/glad.h>
#include <freetype/freetype.h>


const mv::uint mv::Font::char_count{ 128 };
const mv::uint mv::Font::chars_per_row{ 8 };

mv::size_type mv::Font::_instance_count{ 0 };
void* mv::Font::_freetype_lib{ nullptr };


void* mv::Font::freetype_lib()
{
	return _freetype_lib;
}


mv::Font::Font()
	: Texture()
{
	if (_instance_count++ == 0) {
		_freetype_lib = new FT_Library;
		FT_Init_FreeType(static_cast<FT_Library*>(_freetype_lib));
	}
}

mv::Font::~Font()
{
	if (--_instance_count == 0) {
		FT_Done_FreeType(*static_cast<FT_Library*>(_freetype_lib));
		delete _freetype_lib;
	}
}

void mv::Font::set_data(uint texture_handle, uint width, uint height, std::vector<GlyphData>&& glyphs)
{
	this->Texture::set_data(texture_handle, width, height);
	this->_glyphs = std::move(glyphs);
}


const mv::Font::GlyphData& mv::Font::glyph_data(char glyph) const
{
	return this->_glyphs[glyph];
}


void mv::Font::_unload()
{
	this->Texture::_unload();
	this->_glyphs.clear();
}




mv::FontFileLoader::FontFileLoader(const std::string& file_path, uint font_size)
	: _file_path(file_path), _font_size{ font_size }
{}

mv::FontFileLoader::FontFileLoader(std::string&& file_path, uint font_size)
	: _file_path(std::move(file_path)), _font_size{ font_size }
{}


void mv::FontFileLoader::load(Font* resource)
{
	FT_Face face;
	if (FT_New_Face(*static_cast<FT_Library*>(Font::freetype_lib()), this->_file_path.c_str(), 0, &face)) {
		throw "failed to load font";
	}
	FT_Set_Pixel_Sizes(face, 0, this->_font_size);

	uint char_width = static_cast<uint>(static_cast<float>(face->bbox.xMax - face->bbox.xMin) /
		static_cast<float>(face->units_per_EM) * static_cast<float>(this->_font_size)) + 1;
	uint char_height = static_cast<uint>(static_cast<float>(face->bbox.yMax - face->bbox.yMin) /
		static_cast<float>(face->units_per_EM) * static_cast<float>(this->_font_size)) + 1;
	byte* texture_buffer = new byte[Font::char_count * char_width * char_height];
	(void)texture_buffer;
	uint width = char_width * Font::chars_per_row;
	uint height = char_height * Font::char_count / Font::chars_per_row;

	std::vector<Font::GlyphData> glyphs;
	glyphs.reserve(Font::char_count);

	for (size_type c = 0; c < Font::char_count; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			continue;
		}

		uint origin = char_width * (c % Font::chars_per_row + Font::chars_per_row * char_height * (c / Font::chars_per_row));
		for (uint row = 0; row < face->glyph->bitmap.rows; ++row) {
			std::copy(face->glyph->bitmap.buffer + row * face->glyph->bitmap.width,
				face->glyph->bitmap.buffer + (row + 1) * face->glyph->bitmap.width,
				texture_buffer + origin + row * char_width * Font::chars_per_row);
		}

		glyphs.push_back(Font::GlyphData{
			{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
			{ face->glyph->bitmap_left, face->glyph->bitmap_top },
			static_cast<float>(face->glyph->advance.x) / 64.f
		});
	}

	FT_Done_Face(face);

	uint texture_handle;
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, texture_buffer);
	delete[] texture_buffer;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	resource->set_data(texture_handle, width, height, std::move(glyphs));
}
