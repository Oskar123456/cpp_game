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

void menu_button_render(Button& btn, i64 t_start, i64 t_now, i64 dt)
{
    float text_scale = min(btn.width, btn.height) / 100.0f, text_w, text_h;
    float angle = btn.rotation;

    if (btn.t_anim_ms > 0)
        btn.anim(btn, t_start, t_now, dt);

    twod_get_text_dims(btn.text.c_str(), btn.text.length(), text_scale, &text_w, &text_h);
    twod_draw_rectf_rounded(btn.pos.x, btn.pos.y, btn.width, btn.height, btn.border_radius, btn.color_bg, NULL, angle);
    twod_draw_text(btn.text.c_str(), btn.text.length(), btn.pos.x - text_w / 2.0f, btn.pos.y + text_h / 2.0f, text_scale, btn.color_fg, angle);
}

void menu_button_update(Button& btn, i64 t_start, i64 t_now, i64 dt)
{
    if (!btn.anim)
        btn.anim = menu_button_anim_squiggle;

    float mouse_x, mouse_y;
    SDL_MouseButtonFlags mouse_flags = SDL_GetMouseState(&mouse_x, &mouse_y);
    if (mouse_x >= btn.pos.x - btn.width / 2 && mouse_x <= btn.pos.x + btn.width / 2
            && mouse_y >= btn.pos.y - btn.height / 2 && mouse_y <= btn.pos.y + btn.height / 2) {
        if (!btn.hover)
            btn.t_anim_ms = 150;
        btn.hover = true;
    }
    else {
        btn.hover = false;
    }
}

void menu_button_check_if_clicked(Button& btn, float x, float y)
{
    if (!btn.on_click)
        return;

    if (x >= btn.pos.x - btn.width / 2 && x <= btn.pos.x + btn.width / 2
            && y >= btn.pos.y - btn.height / 2 && y <= btn.pos.y + btn.height / 2) {
        btn.on_click();
    }
}

void menu_button_anim_squiggle(Button& btn, i64 t_start, i64 t_now, i64 dt)
{
    btn.t_anim_ms -= dt / 1000000;
    if (btn.t_anim_ms <= 0) {
        btn.rotation = btn.rotation_default;
        btn.t_anim_ms = 0;
        return;
    }

    i64 _dt = t_now - t_start;
    float angle = (_dt / 700000) * (2 * M_PI / 360.0f);
    angle = abs(sin(angle)) * (M_PI / 8) - (M_PI / 16);

    btn.rotation = angle + btn.rotation_default;
}









































