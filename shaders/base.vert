#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_norm;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 o_uv;

void main()
{
	mat4 mvp = u_projection * u_view * u_model;
	const float jitter = 64.0;

	gl_Position = mvp * vec4(a_pos, 1.0);
	gl_Position.xy = floor(gl_Position.xy * jitter) / jitter;
	o_uv = a_uv;
}
