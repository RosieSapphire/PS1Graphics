#ifndef PS1_MESH_H
#define PS1_MESH_H

#include <rmath/vec3f.h>
#include <rmath/vec2f.h>

struct vertex {
	rm_vec3f pos;
	rm_vec2f uv;
	rm_vec3f norm;
};

struct mesh {
	int num_verts, num_indis;
	struct vertex *verts;
	unsigned int *indis;
	unsigned int vao, vbo, ebo;
};

struct mesh *mesh_create_cube(void);
void mesh_draw(struct mesh *m, unsigned int texture);
void mesh_destroy(struct mesh *m);

#endif
