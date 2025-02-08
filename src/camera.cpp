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

using namespace std;

/* screen dims*/
static i32 scr_w;
static i32 scr_h;
static i32 mouse_x_last = -1;
static i32 mouse_y_last = -1;

void camera_update(Camera& cam)
{
    cam.forward.x = cos(glm_rad(cam.yaw)) * cos(glm_rad(cam.pitch));
    cam.forward.y = sin(glm_rad(cam.pitch));
    cam.forward.z = sin(glm_rad(cam.yaw)) * cos(glm_rad(cam.pitch));

    cam.forward = vec3_normalize(cam.forward);
    cam.at = vec3_add(cam.forward, cam.pos);
    cam.left = vec3_normalize(vec3_cross(cam.up, cam.forward));

    cam.left = vec3_normalize(vec3_cross(cam.up, cam.forward));
    mat4s v = glms_lookat(cam.pos, cam.at, cam.up);
    mat4s p = glms_perspective(cam.fov * ((2 * M_PI) / 360.0f), (float)scr_w/scr_h, 0.1f, 500.0f);
    cam.view_proj = mat4_mul(p, v);
}

void camera_free_poll_keys(Camera& cam, const bool *kb_state, float dt)
{
    float ms = kb_state[SDL_SCANCODE_LSHIFT] ? cam.move_speed_sprint : cam.move_speed;

    if (kb_state[SDL_SCANCODE_UP] || kb_state[SDL_SCANCODE_W]) {
        cam.pos = vec3_add(cam.pos, vec3_scale(cam.forward, dt * ms));
        cam.at = vec3_add(cam.at, vec3_scale(cam.forward, dt * ms));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_DOWN] || kb_state[SDL_SCANCODE_S]) {
        cam.pos = vec3_sub(cam.pos, vec3_scale(cam.forward, dt * ms));
        cam.at = vec3_sub(cam.at, vec3_scale(cam.forward, dt * ms));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_LEFT] || kb_state[SDL_SCANCODE_A]) {
        cam.pos = vec3_add(cam.pos, vec3_scale(cam.left, dt * ms));
        cam.at = vec3_add(cam.at, vec3_scale(cam.left, dt * ms));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_RIGHT] || kb_state[SDL_SCANCODE_D]) {
        cam.pos = vec3_sub(cam.pos, vec3_scale(cam.left, dt * ms));
        cam.at = vec3_sub(cam.at, vec3_scale(cam.left, dt * ms));
        camera_update(cam);
    }
}

void camera_free_update_mouse(Camera& cam, float dx, float dy)
{   // TODO: up vector
    if (dx != 0 || dy != 0) {
        cam.yaw += cam.rot_speed * dx;
        cam.pitch -= cam.rot_speed * dy;

        camera_update(cam);
    }
}

void camera_free_poll_mouse(Camera& cam, SDL_MouseButtonFlags mb_flags, int mouse_x, int mouse_y, float dt)
{
}

void camera_update_scr_dims(i32 w, i32 h)
{
    scr_w = w;
    scr_h = h;
}
