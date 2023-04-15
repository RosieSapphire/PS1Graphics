#ifndef PS1_CAMERA_H
#define PS1_CAMERA_H

#include <rmath/mat4.h>

struct camera {
	rm_vec3f eye_pos;
	rm_vec3f look_pos;
};

void camera_get_view_mat4(struct camera c, rm_mat4 out);

#endif
