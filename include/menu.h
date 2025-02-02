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
    vec2s pos;
    int width, height, border_radius;
    float rotation;
};

void menu_button_render(Button& btn);

#endif



