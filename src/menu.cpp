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

void menu_button_render(Button& btn)
{
    float text_scale = min(btn.width, btn.height) / 100.0f, text_w, text_h;
    twod_get_text_dims(btn.text.c_str(), btn.text.length(), text_scale, &text_w, &text_h);
    twod_draw_rectf_rounded(btn.pos.x, btn.pos.y, btn.width, btn.height, btn.border_radius, btn.color_bg, NULL, 0);
    twod_draw_text(btn.text.c_str(), btn.text.length(), btn.pos.x - text_w / 2.0f, btn.pos.y + text_h / 2.0f, text_scale, btn.color_fg, 0);
}









































