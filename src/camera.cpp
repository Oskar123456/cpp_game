/*=======================================
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

#include <twod.h>
#include <threed.h>
#include <camera.h>
#include <logging.h>

#include <map>

using namespace std;

/* screen dims*/
static i32 scr_w;
static i32 scr_h;

void camera_update(Camera& cam)
{
    mat4s v = glms_lookat(cam.pos, cam.at, cam.up);
    mat4s p = glms_perspective(cam.fov * ((2 * M_PI) / 360.0f), (float)scr_w/scr_h, 0.1f, 100.0f);
    cam.view_proj = mat4_mul(p, v);
    cam.forward = vec3_normalize(vec3_sub(cam.at, cam.pos));
    cam.left = vec3_normalize(vec3_cross(cam.up, cam.forward));
}

void camera_free_poll_keys(Camera& cam, const bool *kb_state, float dt)
{
    if (kb_state[SDL_SCANCODE_UP] || kb_state[SDL_SCANCODE_W]) {
        cam.pos = vec3_add(cam.pos, vec3_scale(cam.forward, dt * cam.speed));
        cam.at = vec3_add(cam.at, vec3_scale(cam.forward, dt * cam.speed));
        camera_update(cam);
        /* glms_vec3_print(cam.pos, stdout); */
    }
    if (kb_state[SDL_SCANCODE_DOWN] || kb_state[SDL_SCANCODE_S]) {
        cam.pos = vec3_sub(cam.pos, vec3_scale(cam.forward, dt * cam.speed));
        cam.at = vec3_sub(cam.at, vec3_scale(cam.forward, dt * cam.speed));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_LEFT] || kb_state[SDL_SCANCODE_A]) {
    }
    if (kb_state[SDL_SCANCODE_RIGHT] || kb_state[SDL_SCANCODE_D]) {
    }
}

void camera_free_poll_mouse(Camera& cam, SDL_MouseButtonFlags mb_flags, int m_x, int m_y, float dt)
{
    if (mb_flags & SDL_BUTTON_LEFT) {
    }
}

void camera_update_scr_dims(i32 w, i32 h)
{
    scr_w = w;
    scr_h = h;
}
