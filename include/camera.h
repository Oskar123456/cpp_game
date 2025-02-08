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
    float move_speed = 0.03f;
    float rot_speed = 0.001f;
    float yaw, pitch, roll;
    vec3s euler_angles;
    vec3s pos, at, up, left, forward;
    mat4s view_proj;
};

void camera_update(Camera& cam);
void camera_update_scr_dims(i32 w, i32 h);
void camera_free_poll_keys(Camera& cam, const bool *kb_state, float dt);
void camera_free_poll_mouse(Camera& cam, SDL_MouseButtonFlags mb_flags, int m_x, int m_y, float dt);
void camera_free_update_mouse(Camera& cam, float dx, float dy);

#endif


