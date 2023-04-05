#version 330 core

layout(location = 0) in vec4 a_vert;

out vec2 o_uv;

void main()
{
	gl_Position = vec4(a_vert.xy, 0.0, 1.0);
	o_uv = a_vert.zw;
}
