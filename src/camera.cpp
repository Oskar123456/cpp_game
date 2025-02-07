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
    mat4s p = glms_perspective(cam.fov * ((2 * M_PI) / 360.0f), 5.0f/4.0f, 0.1f, 100.0f);
    cam.view_proj = mat4_mul(p, v);
}

void camera_update_scr_dims(i32 w, i32 h)
{
    scr_w = w;
    scr_h = h;
}
