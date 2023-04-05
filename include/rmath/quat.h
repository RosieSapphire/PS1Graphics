#ifndef RMATH_QUAT_H
#define RMATH_QUAT_H

#include "rmath/vec3f.h"
#include "rmath/vec4f.h"

void rm_quat_slerp(rm_vec4f a, rm_vec4f b, float t, rm_vec4f out);

#endif
