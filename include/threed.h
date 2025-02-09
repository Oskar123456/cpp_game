/*=======================================
 *
 * cpp_game
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *=======================================*/

#ifndef THREED_H
#define THREED_H

#include <twod.h>

void threed_init();
u32 threed_create_tex(const char* img_path, const char* alias, int format);
void threed_render_cube(mat4s& view_proj, vec3s& pos, float angle, vec3s rot_axis, Color col, u32 vao, u32 tex_id);
u32 threed_create_cube_vao(vec2s (&tex_coords)[24]);
float* threed_get_quad_vertices(int dir);
u32* threed_get_quad_indices(int dir);
u32 threed_get_default_shader();

#endif

