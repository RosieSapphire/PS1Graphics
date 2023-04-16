#include <math.h>
#include "camera.h"
#include "rmath/mat4.h"
#include "rmath/vec3f.h"

void camera_get_look_pos(struct camera c, rm_vec3f out)
{
	rm_vec3f look_pos = {cosf(c.yaw), 0.0f, sinf(c.yaw)};

	rm_vec3f_add(c.eye_pos, look_pos, out);
}

void camera_get_look_dir(struct camera c, rm_vec3f out)
{
	rm_vec3f look_pos;

	camera_get_look_pos(c, look_pos);
	rm_vec3f_sub(look_pos, c.eye_pos, out);
	rm_vec3f_normalize(out);
}

void camera_get_view_mat4(struct camera c, rm_mat4 out)
{
	rm_vec3f look_pos;

	camera_get_look_pos(c, look_pos);
	rm_mat4_look_at(c.eye_pos, look_pos, out);
	camera_get_look_dir(c, look_pos);
}

void camera_update_position(struct camera *c, GLFWwindow *win, float dt)
{
	float move_speed = 2 * dt;

	if(glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)) {
		move_speed *= 2;
	}

	float turn_amount = (glfwGetKey(win, GLFW_KEY_D) -
			glfwGetKey(win, GLFW_KEY_A)) * move_speed;
	float v_move_amount = (glfwGetKey(win, GLFW_KEY_S) -
			glfwGetKey(win, GLFW_KEY_W)) * move_speed;

	rm_vec3f v_move_vec;

	camera_get_look_dir(*c, v_move_vec);
	rm_vec3f_scale(v_move_vec, v_move_amount);
	rm_vec3f_add(c->eye_pos, v_move_vec, c->eye_pos);

	c->yaw += turn_amount;
}
