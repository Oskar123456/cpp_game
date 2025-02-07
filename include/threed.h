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

void threed_init();
u32  threed_create_tex(const char* img_path, const char* alias);
void threed_render_cube(mat4s& view_proj, vec3s& pos, float angle, vec3s rot_axis, Color col, const char* tex);

#endif

