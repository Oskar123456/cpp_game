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

#ifndef TWOD_H
#define TWOD_H

#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include <cglm/struct.h>
#include <util.h>

#define VEC4EXP(v) (v).x, (v).y, (v).z, (v).w

typedef vec4s Color;

void twod_init();
void twod_update_scr_dims(i32 w, i32 h);
void twod_draw_rectf(float x, float y, float w, float h, Color col, float angle);
void twod_draw_rectv(vec2s a, float w, float h, Color c);
void twod_draw_circlef(float x, float y, float r, Color c);
void twod_draw_text(const char* txt, u32 txt_len, float x, float y, float sz, Color c, float angle);
void twod_draw_rectf_tex(float x, float y, float w, float h, Color c, const char* tex, float angle);
void twod_draw_rectf_tex_rot(float x, float y, float w, float h, Color c, const char* tex, float angle);
void twod_draw_rectf_rounded(float x, float y, float w, float h, float border_radius, Color col, const char* tex, float angle);
u32 twod_create_tex(const char* img_path, const char* alias);
u32 twod_create_tex_a(const char* img_path, const char* alias);
float twod_get_text_length(const char* txt, u32 txt_len, float scale);
void twod_get_text_dims(const char* txt, u32 txt_len, float scale, float* w, float* h);
void screenshot();

#define COL_WHITE {0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f, 1.0f}
#define COL_TOKYO {0x1A / 255.0f, 0x1B / 255.0f, 0x26 / 255.0f, 1.0f}
#define COL_PINK {0xFF / 255.0f, 0x00 / 255.0f, 0x7F / 255.0f, 1.0f}
#define COL_BLACK {0x16 / 255.0f, 0x16 / 255.0f, 0x16 / 255.0f, 1.0f}

#endif
