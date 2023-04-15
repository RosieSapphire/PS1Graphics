#ifndef PS1_SHADER_H
#define PS1_SHADER_H

unsigned int shader_compile(const char *path, int type);
unsigned int shader_create(const char *vert_path, const char *frag_path);

#endif
