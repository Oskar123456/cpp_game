/*=======================================
 *
 * menu
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

#ifndef MENU_H
#define MENU_H

#include "twod.h"

#include <string>

struct Button {
    std::string text;
    Color color_fg, color_bg;
    Color color_fg_active, color_bg_active;
    vec2s pos;
    int width, height, border_radius;
    float rotation_default, rotation;
    i64 t_anim_ms;
    void (*anim)(Button&, i64, i64, i64);
    void (*on_click)(void);
    bool active, hover;
};

struct Menu {
    Button* buttons;
    int num_buttons;
};

void menu_update();
void menu_render();
void menu_button_render(Button& btn, i64 t_start, i64 t_now, i64 dt);
void menu_button_update(Button& btn, i64 t_start, i64 t_now, i64 dt);
void menu_button_check_if_clicked(Button& btn, float x, float y);
void menu_button_anim_squiggle(Button& btn, i64 t_start, i64 t_now, i64 dt);

#endif



