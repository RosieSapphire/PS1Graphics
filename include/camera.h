#ifndef PS1_CAMERA_H
#define PS1_CAMERA_H

#include <GLFW/glfw3.h>
#include <rmath/mat4.h>

struct camera {
	rm_vec3f eye_pos;
	float yaw, pitch;
};

void camera_get_look_pos(struct camera c, rm_vec3f out);
void camera_get_look_dir(struct camera c, rm_vec3f out);
void camera_get_view_mat4(struct camera c, rm_mat4 out);
void camera_update_position(struct camera *c, GLFWwindow *win, float dt);

#endif
