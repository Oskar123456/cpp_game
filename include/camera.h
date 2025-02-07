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

#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>

struct Camera {
    float fov;
    float speed = 0.1f;
    vec3s pos, at, up, left, forward;
    mat4s view_proj;
};

void camera_update(Camera& cam);
void camera_update_scr_dims(i32 w, i32 h);
void camera_free_poll_keys(Camera& cam, const bool *kb_state, float dt);
void camera_free_poll_mouse(Camera& cam, SDL_MouseButtonFlags mb_flags, int m_x, int m_y, float dt);

#endif


