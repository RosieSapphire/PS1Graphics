#include <stdio.h>
#include <malloc.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "rmath/vec3f.h"
#include "rmath/vec2f.h"
#include "rmath/mat4.h"

#define WIDTH 1024
#define HEIGHT 768
#define T_WIDTH (WIDTH >> 2)
#define T_HEIGHT (HEIGHT >> 2)
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)

struct vertex {
	rm_vec3f pos;
	rm_vec2f uv;
	rm_vec3f norm;
};

struct vertex obj_verts[4] = {
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
};

GLuint obj_indis[6] = {
	0, 1, 2,
	2, 1, 3
};

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
GLuint texture_load(const char *path);

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
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	GLuint obj_vao, obj_vbo, obj_ebo;

	glGenVertexArrays(1, &obj_vao);
	glBindVertexArray(obj_vao);
	glGenBuffers(1, &obj_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, obj_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(obj_verts),
			obj_verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(struct vertex),
			(void *)offsetof(struct vertex, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(struct vertex),
			(void *)offsetof(struct vertex, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(struct vertex),
			(void *)offsetof(struct vertex, norm));

	glGenBuffers(1, &obj_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(obj_indis),
			obj_indis, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	GLuint obj_shader =
		shader_create("shaders/base.vert", "shaders/base.frag");
	GLuint fbo_shader =
		shader_create("shaders/fbo.vert", "shaders/fbo.frag");
	GLuint texture = texture_load("textures/test.png");

	int projection_loc = glGetUniformLocation(obj_shader, "u_projection");
	int view_loc = glGetUniformLocation(obj_shader, "u_view");
	int view_pos_loc = glGetUniformLocation(obj_shader, "u_view_pos");
	int model_loc = glGetUniformLocation(obj_shader, "u_model");

	int width_loc = glGetUniformLocation(fbo_shader, "u_width");
	int height_loc = glGetUniformLocation(fbo_shader, "u_height");

	rm_mat4 projection;
	rm_mat4 view = RM_MAT4_IDENTITY_INIT;
	rm_vec3f view_pos = {0, 0, -2};
	rm_mat4 model = RM_MAT4_IDENTITY_INIT;

	rm_mat4_perspective(50.0f, ASPECT_RATIO, 1, 50, projection);
	rm_mat4_translate(view, view_pos);

	GLuint fbo;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint fbo_tex;

	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, T_WIDTH, T_HEIGHT, 0, GL_RGB,
			GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, fbo_tex, 0);

	GLuint rbo;

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

	float time_last = glfwGetTime();

	while(!glfwWindowShouldClose(window)) {
		float time_now, time_delta;

		do {
			time_now = glfwGetTime();
			time_delta = time_now - time_last;
		} while(time_delta < 1.0f / 30.0f);

		time_last = time_now;

		rm_mat4_rotate_x(model, 1.3f * time_delta);
		rm_mat4_rotate_z(model, 1.0f * time_delta);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, T_WIDTH, T_HEIGHT);
		glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(obj_shader);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE,
				(const float *)model);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE,
				(const float *)view);
		glUniform3fv(view_pos_loc, 1, (const float *)view_pos);
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE,
				(const float *)projection);
		glBindVertexArray(obj_vao);
		glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES,
				sizeof(obj_indis) / sizeof(*obj_indis),
				GL_UNSIGNED_INT, obj_indis);

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
		glEnable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES,
				sizeof(fbo_indis) / sizeof(*fbo_indis),
				GL_UNSIGNED_INT, fbo_indis);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(fbo_shader);
	glDeleteProgram(obj_shader);
	glDeleteVertexArrays(1, &obj_vao);
	glDeleteBuffers(1, &obj_vbo);
	glDeleteTextures(1, &texture);
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

GLuint texture_load(const char *path)
{
	GLuint id;
	int width, height, comp;

	stbi_set_flip_vertically_on_load(1);

	uint8_t *data = stbi_load(path, &width, &height, &comp, 0);

	if(!data) {
		printf("Failed to load texture from '%s'\n", path);
		return 0;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	return id;
}
