#include "camera.h"
#include "rmath/mat4.h"

void camera_get_view_mat4(struct camera c, rm_mat4 out)
{
	rm_mat4_look_at(c.eye_pos, c.look_pos, out);
}
