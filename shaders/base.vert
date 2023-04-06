#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_norm;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 o_uv;
out vec3 o_ambient;
out vec3 o_diffuse;
out vec3 o_light_col;

void main()
{
	mat4 mvp = u_projection * u_view * u_model;
	const float jitter = 64.0;
	vec4 vertex_pos = vec4(a_pos, 1.0);

	gl_Position = mvp * vertex_pos;
	gl_Position.xyz = floor(gl_Position.xyz * jitter) / jitter;
	gl_Position /= gl_Position.w;

	vec3 frag_pos = vec3(u_model * vertex_pos);
	vec3 light_col = vec3(1);
	vec3 light_dir = normalize(vec3(2) - frag_pos);
	vec3 norm = mat3(transpose(inverse(u_model))) *
			normalize(a_norm);

	o_uv = a_uv;
	o_light_col = vec3(1);
	o_ambient = 0.1 * o_light_col;
	o_diffuse = max(dot(norm, light_dir), 0.0) * light_col;
}
