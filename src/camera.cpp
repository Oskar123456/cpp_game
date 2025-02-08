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
static i32 mouse_x_last = -1;
static i32 mouse_y_last = -1;

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
        cam.pos = vec3_add(cam.pos, vec3_scale(cam.forward, dt * cam.move_speed));
        cam.at = vec3_add(cam.at, vec3_scale(cam.forward, dt * cam.move_speed));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_DOWN] || kb_state[SDL_SCANCODE_S]) {
        cam.pos = vec3_sub(cam.pos, vec3_scale(cam.forward, dt * cam.move_speed));
        cam.at = vec3_sub(cam.at, vec3_scale(cam.forward, dt * cam.move_speed));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_LEFT] || kb_state[SDL_SCANCODE_A]) {
        cam.pos = vec3_add(cam.pos, vec3_scale(cam.left, dt * cam.move_speed));
        cam.at = vec3_add(cam.at, vec3_scale(cam.left, dt * cam.move_speed));
        camera_update(cam);
    }
    if (kb_state[SDL_SCANCODE_RIGHT] || kb_state[SDL_SCANCODE_D]) {
        cam.pos = vec3_sub(cam.pos, vec3_scale(cam.left, dt * cam.move_speed));
        cam.at = vec3_sub(cam.at, vec3_scale(cam.left, dt * cam.move_speed));
    }
}

void camera_free_update_mouse(Camera& cam, float dx, float dy)
{
    /* if (mouse_x_last < 0 || mouse_y_last < 0) { */
    /*     mouse_x_last = mouse_x; */
    /*     mouse_y_last = mouse_y; */
    /* } */

    if (dx != 0 || dy != 0) {
        printf("%f %f %f\n", dx, dy, cam.rot_speed);
        /* yaw = dy * dt; */
        /* pitch = dx * dt; */
        vec3s euler_angles = {dy * cam.rot_speed, dx * cam.rot_speed, 0};
        mat4s r = GLMS_MAT4_IDENTITY;
        glm_euler(euler_angles.raw, r.raw);
        cam.at = vec3_add(mat4_mulv3(r, vec3_sub(cam.at, cam.pos), 1), cam.pos);
        cam.up = vec3_add(mat4_mulv3(r, vec3_sub(vec3_sub(cam.up, cam.pos), cam.pos), 1), cam.pos);
        cam.left = vec3_add(mat4_mulv3(r, vec3_sub(cam.left, cam.pos), 1), cam.pos);
    }
}

void camera_free_poll_mouse(Camera& cam, SDL_MouseButtonFlags mb_flags, int mouse_x, int mouse_y, float dt)
{
    if (mouse_x_last < 0 || mouse_y_last < 0) {
        mouse_x_last = mouse_x;
        mouse_y_last = mouse_y;
    }

    int dx = mouse_x - mouse_x_last;
    int dy = mouse_y - mouse_y_last;

    mouse_x_last = mouse_x;
    mouse_y_last = mouse_y;

    if (mb_flags & SDL_BUTTON_LEFT) {

    }

    if (dx != 0 || dy != 0) {
        printf("%d %d %f\n", dx, dy, cam.rot_speed);
        /* yaw = dy * dt; */
        /* pitch = dx * dt; */
        vec3s euler_angles = {dy * dt * cam.rot_speed, dx * dt * cam.rot_speed, 0};
        mat4s r = GLMS_MAT4_IDENTITY;
        glm_euler(euler_angles.raw, r.raw);
        cam.at = vec3_add(mat4_mulv3(r, vec3_sub(cam.at, cam.pos), 1), cam.pos);
        cam.up = vec3_add(mat4_mulv3(r, vec3_sub(vec3_sub(cam.up, cam.pos), cam.pos), 1), cam.pos);
        cam.left = vec3_add(mat4_mulv3(r, vec3_sub(cam.left, cam.pos), 1), cam.pos);
    }
}

void camera_update_scr_dims(i32 w, i32 h)
{
    scr_w = w;
    scr_h = h;
}
