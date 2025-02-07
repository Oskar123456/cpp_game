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

struct Camera {
    float fov;
    vec3s pos, at, up, left;
    mat4s view_proj;
};

void camera_update(Camera& cam);

#endif


