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

typedef vec4s Color;

void twod_init();
void twod_update_scr_dims(i32 w, i32 h);
void twod_draw_rectf(float x, float y, float w, float h, Color c);
void twod_draw_rectv(vec2s a, float w, float h, Color c);

#endif
