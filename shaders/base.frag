#version 330 core

in vec2 o_uv;

uniform sampler2D u_tex;
uniform vec3 u_view_pos;

out vec4 frag_color;

void main()
{
	frag_color = texture(u_tex, o_uv);
}
