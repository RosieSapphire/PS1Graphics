#version 330 core

in vec2 o_uv;
in vec3 o_frag_pos;

uniform sampler2D u_tex;
uniform vec3 u_view_pos;

out vec4 frag_color;

void main()
{
	vec3 light_dir = normalize(vec3(1));
	vec3 cam_dir = normalize(o_frag_pos - u_view_pos);
	float diffuse = max(dot(cam_dir, light_dir), 0);

	frag_color = texture(u_tex, o_uv) * diffuse;
}
