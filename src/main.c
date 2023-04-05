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
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)

struct vertex {
	rm_vec3f pos;
	rm_vec2f uv;
	rm_vec3f norm;
};

struct vertex vertices[6] = {
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
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
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
			vertices, GL_STATIC_DRAW);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLuint shader = shader_create("shaders/base.vert", "shaders/base.frag");
	GLuint texture = texture_load("textures/test.png");

	int projection_loc = glGetUniformLocation(shader, "u_projection");
	int view_loc = glGetUniformLocation(shader, "u_view");
	int view_pos_loc = glGetUniformLocation(shader, "u_view_pos");
	int model_loc = glGetUniformLocation(shader, "u_model");

	rm_mat4 projection;
	rm_mat4 view = RM_MAT4_IDENTITY_INIT;
	rm_vec3f view_pos = {0.0f, 0.0f, -2.0f};
	rm_mat4 model = RM_MAT4_IDENTITY_INIT;

	rm_mat4_perspective(90.0f, ASPECT_RATIO, 0.1f, 3.0f, projection);
	rm_mat4_translate(view, view_pos);

	while(!glfwWindowShouldClose(window)) {
		rm_vec3f move = {0.001f, 0.002f, 0.0f};

		rm_mat4_translate(model, move);
		rm_mat4_rotate_x(model, 0.013f);
		rm_mat4_rotate_z(model, 0.01f);

		glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shader);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE,
				(const float *)model);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE,
				(const float *)view);
		glUniform3fv(view_pos_loc, 1, (const float *)view_pos);
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE,
				(const float *)projection);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, (sizeof(vertices) >> 2));
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);
	glDeleteTextures(1, &texture);
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

	glActiveTexture(GL_TEXTURE0);
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
