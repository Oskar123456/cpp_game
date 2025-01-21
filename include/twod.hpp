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

#ifndef TWOD_HPP
#define TWOD_HPP

#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include <cglm/struct.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <util.hpp>

#define VEC4EXP(v) (v).x, (v).y, (v).z, (v).w

typedef vec4s Color;

void twod_init();
void twod_update_scr_dims(i32 w, i32 h);
void twod_draw_rectf(float x, float y, float w, float h, Color c);
void twod_draw_rectv(vec2s a, float w, float h, Color c);

#define COL_TOKYO {0x1A / 255.0f, 0x1B / 255.0f, 0x26 / 255.0f, 1.0f}
#define COL_PINK {0xFF / 255.0f, 0x00 / 255.0f, 0x7F / 255.0f, 1.0f}

#endif
