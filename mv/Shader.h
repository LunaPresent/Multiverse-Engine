#pragma once
#include "setup.h"

namespace mv
{
	class Shader
	{
	private:
		uint _program_id;


	public:
		Shader(const char* vertex_source, const char* fragment_source);

	};
}
