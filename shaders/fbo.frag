#version 330 core

in vec2 o_uv;

uniform sampler2D u_tex;

out vec4 frag_color;

const int bayer[4 * 4] = int[] (
	0, 8, 2, 10,
	12, 4, 14, 6,
	3, 11, 1, 9,
	15, 7, 13, 5
);

void main()
{
	const float bit_depth = 8 - 1;
	vec3 tex = texture(u_tex, o_uv).xyz;
	vec3 tex_low = vec3(ivec3(tex * bit_depth)) / bit_depth;

	frag_color = vec4(tex_low, 1.0);
}
