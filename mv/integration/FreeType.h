#pragma once
#include "../Singleton.h"

#include <mutex>

#include <freetype/freetype.h>

#include "../Multiverse.h"

namespace mv
{
	class FreeType final : public Singleton<FreeType>
	{
		friend Multiverse;

	private:
		FT_Library _freetype_lib;
		std::mutex _face_mtx;

		FreeType();
		~FreeType();

	public:
		FT_Face create_face(const char* file_path, uint face_index = 0);
		void destroy_face(FT_Face face);
	};
}
