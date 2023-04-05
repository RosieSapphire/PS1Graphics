#version 330 core

in vec2 o_uv;

uniform sampler2D u_tex;
uniform int u_width;
uniform int u_height;

out vec4 frag_color;

const int bayer[4 * 4] = int[] (
	0, 8, 2, 10,
	12, 4, 14, 6,
	3, 11, 1, 9,
	15, 7, 13, 5
);

void main()
{
	ivec2 pixel = ivec2(o_uv * vec2(u_width, u_height));
	int bayer_index = (pixel.y % 4) * 4 + (pixel.x % 4);
	float bayer_value = (float(bayer[bayer_index]) / 16.0) - 0.5;

	const float bit_depth = 31;
	const float spread = 0.4;
	vec3 tex = texture(u_tex, o_uv).xyz;
	tex += bayer_value * spread;
	vec3 tex_low = vec3(ivec3(tex * bit_depth)) / bit_depth;

	frag_color = vec4(tex_low, 1.0);
}
