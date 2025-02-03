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

#ifndef MODEL_H
#define MODEL_H

#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include <cglm/struct.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <util.h>

struct Transform {
    mat4s rotation;
    vec3s position;
    vec3s scale;
};

struct Model {

};

typedef vec4s Color;

#endif

