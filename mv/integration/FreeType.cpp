#include "pch.h"
#include "FreeType.h"


mv::FreeType::FreeType()
{
	FT_Init_FreeType(&this->_freetype_lib);
}

mv::FreeType::~FreeType()
{
	FT_Done_FreeType(this->_freetype_lib);
}


FT_Face mv::FreeType::create_face(const char* file_path, uint face_index)
{
	FT_Face face;
	this->_face_mtx.lock();
	FT_New_Face(this->_freetype_lib, file_path, static_cast<FT_Long>(face_index), &face);
	this->_face_mtx.unlock();
	return face;
}

void mv::FreeType::destroy_face(FT_Face face)
{
	this->_face_mtx.lock();
	FT_Done_Face(face);
	this->_face_mtx.unlock();
}
