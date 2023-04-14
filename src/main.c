#include <stdio.h>
#include <malloc.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "rmath/vec3f.h"
#include "rmath/vec2f.h"
#include "rmath/mat4.h"

#include "texture.h"
#include "mesh.h"

#define WIDTH 1024
#define HEIGHT 768
#define TARGET_FRAMERATE 30.0f
#define T_WIDTH (WIDTH >> 2)
#define T_HEIGHT (HEIGHT >> 2)
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)

rm_vec4f fbo_verts[4] = {
	{-1.0f, -1.0f, 0.0f, 0.0f},
	{ 1.0f, -1.0f, 1.0f, 0.0f},
	{-1.0f,  1.0f, 0.0f, 1.0f},
	{ 1.0f,  1.0f, 1.0f, 1.0f},
};

GLuint fbo_indis[6] = {
	0, 1, 2,
	2, 1, 3
};

char *file_read_text(const char *path);
GLuint shader_compile(const char *path, int type);
GLuint shader_create(const char *vert_path, const char *frag_path);
GLFWwindow *window_create_centered(int width, int height, const char *title);

int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = window_create_centered(WIDTH, HEIGHT,
			"PS1 Graphics Test");

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glEnable(GL_DEPTH);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	GLuint fbo_vao, fbo_vbo, fbo_ebo;

	glGenVertexArrays(1, &fbo_vao);
	glBindVertexArray(fbo_vao);
	glGenBuffers(1, &fbo_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, fbo_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_verts),
			fbo_verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(rm_vec4f), NULL);

	glGenBuffers(1, &fbo_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbo_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fbo_indis),
			fbo_indis, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLuint cube_shader =
		shader_create("shaders/base.vert", "shaders/base.frag");
	GLuint fbo_shader =
		shader_create("shaders/fbo.vert", "shaders/fbo.frag");

	int projection_loc = glGetUniformLocation(cube_shader, "u_projection");
	int view_loc = glGetUniformLocation(cube_shader, "u_view");
	int view_pos_loc = glGetUniformLocation(cube_shader, "u_view_pos");
	int model_loc = glGetUniformLocation(cube_shader, "u_model");

	int width_loc = glGetUniformLocation(fbo_shader, "u_width");
	int height_loc = glGetUniformLocation(fbo_shader, "u_height");

	rm_mat4 projection;
	rm_mat4 view = RM_MAT4_IDENTITY_INIT;
	rm_vec3f view_pos = {0, -1, -2};

	rm_mat4_perspective(70.0f, ASPECT_RATIO, 1, 50, projection);
	rm_mat4_look_at(view_pos, RM_VEC3F_ZERO, view);

	GLuint fbo, rbo, fbo_tex = texture_create_empty(T_WIDTH, T_HEIGHT);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, fbo_tex, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER,
			GL_DEPTH24_STENCIL8, T_WIDTH, T_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)
			!= GL_FRAMEBUFFER_COMPLETE) {
		printf("Failed to create framebuffer.\n");
		return 1;
	}

	struct mesh *cube_mesh = mesh_create_cube();
	GLuint crate_texture = texture_load("textures/test3.png");

	float time_last = glfwGetTime();
	float rotation = 0.0f;

	while(!glfwWindowShouldClose(window)) {
		float time_now, time_delta;
		rm_mat4 model = RM_MAT4_IDENTITY_INIT;

		do {
			time_now = glfwGetTime();
			time_delta = time_now - time_last;
		} while(time_delta < 1.0f / TARGET_FRAMERATE);

		time_last = time_now;

		rotation += time_delta;
		rm_mat4_rotate_y(model, rotation);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, T_WIDTH, T_HEIGHT);
		glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(cube_shader);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE,
				(const float *)model);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE,
				(const float *)view);
		glUniform3fv(view_pos_loc, 1, (const float *)view_pos);
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE,
				(const float *)projection);

		mesh_draw(cube_mesh, crate_texture);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0,
				T_WIDTH, T_HEIGHT, GL_COLOR_BUFFER_BIT,
				GL_NEAREST);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(fbo_shader);
		glUniform1i(width_loc, T_WIDTH);
		glUniform1i(height_loc, T_HEIGHT);
		glBindVertexArray(fbo_vao);
		glBindTexture(GL_TEXTURE_2D, fbo_tex);
		glDrawElements(GL_TRIANGLES,
				sizeof(fbo_indis) / sizeof(*fbo_indis),
				GL_UNSIGNED_INT, fbo_indis);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(fbo_shader);
	glDeleteProgram(cube_shader);
	glDeleteTextures(1, &crate_texture);
	mesh_destroy(cube_mesh);
	glDeleteFramebuffers(1, &fbo);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

char *file_read_text(const char *path)
{
	FILE *file = fopen(path, "r");
	size_t file_size = 0L;
	char *buffer = NULL;

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);
	buffer = malloc(file_size);
	fread(buffer, sizeof(char), file_size, file);
	fclose(file);
	buffer[--file_size] = 0;

	return buffer;
}

GLuint shader_compile(const char *path, int type)
{
	GLuint shader = glCreateShader(type);
	char *source = file_read_text(path);
	GLint status;

	glShaderSource(shader, 1, (const char **)&source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if(status)
		return shader;

	char log[512];

	glGetShaderInfoLog(shader, 512, NULL, log);
	printf("Failed to create shader from '%s': %s\n", path, log);
	free(source);

	return 0;
}

GLuint shader_create(const char *vert_path, const char *frag_path)
{
	GLuint vert_shader = shader_compile(vert_path, GL_VERTEX_SHADER);
	GLuint frag_shader = shader_compile(frag_path, GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();

	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return program;
}

GLFWwindow *window_create_centered(int width, int height, const char *title)
{
	GLFWwindow *window = glfwCreateWindow(width, height, title,
			NULL, NULL);

	glfwMakeContextCurrent(window);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	int monitor_w, monitor_h;
	int window_x, window_y;

	glfwGetMonitorWorkarea(monitor, NULL, NULL, &monitor_w, &monitor_h);
	window_x = (monitor_w >> 1) - (width >> 1);
	window_y = (monitor_h >> 1) - (height >> 1);
	glfwSetWindowPos(window, window_x, window_y);

	return window;
}
