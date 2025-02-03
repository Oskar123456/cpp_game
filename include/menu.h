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
    float rotation;
    bool active;
};

void menu_button_render(Button& btn, i64 t_start, i64 t_now);
void menu_button_update(Button& btn, i64 t_start, i64 t_now);

#endif



