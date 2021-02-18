#version 460 core
in vec2 _tex_coords;

uniform sampler2D char_map;
uniform vec3 text_colour;

out vec4 frag_colour;

void main()
{
	frag_colour = vec4(1.0, 1.0, 1.0, texture(char_map, _tex_coords).r);
}
