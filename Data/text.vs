#version 460 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 _tex_coords;

void main()
{
	gl_Position = proj * view * model * vec4(pos, 0.0, 1.0);
	_tex_coords = tex_coords;
}
