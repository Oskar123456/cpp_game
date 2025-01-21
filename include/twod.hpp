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

#include <util.hpp>
#include <glad/glad.h>
#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include <cglm/struct.h>

typedef vec4s Color;

void twod_init();
void twod_draw_rectf(float ax, float ay, float bx, float by, Color c);
void twod_draw_rectv(vec2s a, vec2s b, Color c);

#endif
