#ifndef RM_RAY_H
#define RM_RAY_H

#include "rmath/vec3f.h"

uint8_t ncx_ray_triangle(rm_vec3f eye, rm_vec3f dir,
		rm_vec3f *verts, float *distance);

#endif
