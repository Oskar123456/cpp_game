/*=======================================
 *
 * tetris
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

#include "menu.h"

using namespace std;

void menu_button_render(Button& btn, i64 t_start, i64 t_now)
{
    float text_scale = min(btn.width, btn.height) / 100.0f, text_w, text_h;
    float angle = btn.rotation;

    if (btn.active) {
        i64 dt = t_now - t_start;
        angle = (dt / 800000) * (2 * M_PI / 360.0f);
        angle = abs(sin(angle)) * (M_PI / 8) - (M_PI / 16);
    }

    twod_get_text_dims(btn.text.c_str(), btn.text.length(), text_scale, &text_w, &text_h);
    twod_draw_rectf_rounded(btn.pos.x, btn.pos.y, btn.width, btn.height, btn.border_radius, btn.color_bg, NULL, angle);
    twod_draw_text(btn.text.c_str(), btn.text.length(), btn.pos.x - text_w / 2.0f, btn.pos.y + text_h / 2.0f, text_scale, btn.color_fg, angle);
}

void menu_button_update(Button& btn, i64 t_start, i64 t_now)
{
    float mouse_x, mouse_y;
    SDL_MouseButtonFlags mouse_flags = SDL_GetMouseState(&mouse_x, &mouse_y);
    if (mouse_x >= btn.pos.x - btn.width / 2 && mouse_x <= btn.pos.x + btn.width / 2
            && mouse_y >= btn.pos.y - btn.height / 2 && mouse_y <= btn.pos.y + btn.height / 2) {
        btn.active = true;
    }
    else {
        btn.active = false;
    }
}









































