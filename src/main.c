#include <stdio.h>
#include <malloc.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "rmath/vec3f.h"
#include "rmath/vec2f.h"
#include "rmath/mat4.h"

#include "texture.h"
#include "mesh.h"
#include "render_layer.h"
#include "shader.h"

#define WIDTH 1024
#define HEIGHT 768
#define TARGET_FRAMERATE 30.0f
#define T_WIDTH (WIDTH >> 2)
#define T_HEIGHT (HEIGHT >> 2)
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)

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

	struct render_layer *layer = render_layer_create(T_WIDTH, T_HEIGHT);

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

	struct mesh *cube_mesh = mesh_create_type(MESH_CUBE);
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

		render_layer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);

		glUseProgram(cube_shader);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);
		glUniform3fv(view_pos_loc, 1, (const float *)view_pos);
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE,
				(float *)projection);
		mesh_draw(cube_mesh, crate_texture);

		glUseProgram(fbo_shader);
		glUniform1i(width_loc, T_WIDTH);
		glUniform1i(height_loc, T_HEIGHT);
		render_layer_draw(layer, WIDTH, HEIGHT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(fbo_shader);
	glDeleteProgram(cube_shader);
	glDeleteTextures(1, &crate_texture);
	mesh_destroy(cube_mesh);
	render_layer_destroy(layer);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
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
