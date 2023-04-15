#include <glad/gl.h>
#include <malloc.h>
#include <string.h>

#include <rmath/vec4f.h>

#include "mesh.h"

static struct vertex mesh_cube_verts[] = {
	/* front */
	{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, { 0.0f,  0.0f,  1.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, { 0.0f,  0.0f,  1.0f}},
	{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, { 0.0f,  0.0f,  1.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f}},

	/* left */
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},
	{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},
	{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},
	{{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},

	/* right */
	{{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, { 1.0f,  0.0f,  0.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, { 1.0f,  0.0f,  0.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, { 1.0f,  0.0f,  0.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, { 1.0f,  0.0f,  0.0f}},

	/* back */
	{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}},

	/* bottom */
	{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, { 0.0f, -1.0f,  0.0f}},
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, { 0.0f, -1.0f,  0.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},

	/* top */
	{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, { 0.0f,  1.0f,  0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
	{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, { 0.0f,  1.0f,  0.0f}},
	{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
};

static GLuint mesh_cube_indis[] = {
	0, 1, 2,
	2, 1, 3,

	4, 5, 6,
	6, 5, 7,

	8, 9, 10,
	10, 9, 11,

	12, 13, 14,
	14, 13, 15,

	16, 17, 18,
	18, 17, 19,

	20, 21, 22,
	22, 21, 23,
};

static struct vertex mesh_quad_verts[] = {
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
};

static GLuint mesh_quad_indis[6] = {
	0, 1, 2,
	2, 1, 3
};

struct mesh *mesh_create_type(enum mesh_type type)
{
	struct mesh *m = malloc(sizeof(*m));

	struct vertex *type_verts[MESH_TYPE_COUNT] = {
		mesh_cube_verts,
		mesh_quad_verts
	};

	GLuint *type_indis[MESH_TYPE_COUNT] = {
		mesh_cube_indis,
		mesh_quad_indis
	};

	size_t type_verts_sizes[MESH_TYPE_COUNT] = {
		sizeof(mesh_cube_verts),
		sizeof(mesh_quad_verts),
	};

	size_t type_indis_sizes[MESH_TYPE_COUNT] = {
		sizeof(mesh_cube_indis),
		sizeof(mesh_quad_indis),
	};

	int verts_size = type_verts_sizes[type];
	int indis_size = type_indis_sizes[type];
	int num_verts = verts_size / sizeof(struct vertex);
	int num_indis = indis_size / sizeof(GLuint);

	m->num_verts = num_verts;
	m->num_indis = num_indis;
	m->verts = malloc(verts_size);
	m->indis = malloc(indis_size);
	memcpy(m->verts, type_verts[type], verts_size);
	memcpy(m->indis, type_indis[type], indis_size);

	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);
	glGenBuffers(1, &m->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);

	glBufferData(GL_ARRAY_BUFFER, verts_size, m->verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*m->verts),
			(void *)offsetof(struct vertex, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*m->verts),
			(void *)offsetof(struct vertex, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*m->verts),
			(void *)offsetof(struct vertex, norm));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m->indis),
			m->indis, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return m;
}

void mesh_draw(struct mesh *m, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(m->vao);
	glDrawElements(GL_TRIANGLES, m->num_indis, GL_UNSIGNED_INT, m->indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void mesh_destroy(struct mesh *m)
{
	glDeleteVertexArrays(1, &m->vao);
	glDeleteBuffers(1, &m->vbo);
	glDeleteBuffers(1, &m->ebo);
	free(m->verts);
	free(m->indis);
	free(m);
}
