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

#include "../include/twod.hpp"

/* screen dims*/
static i32 scr_w;
static i32 scr_h;
/* shaders */
static u32 shdr_simp;
static u32 shdr_simp_loc_col;
static u32 shdr_simp_loc_modl;
/* vertex data */
static u32 vao_rect;

static float vs_rect[] = {
     0.0,  0.0, 0.0,
     0.0,  1.0, 0.0,
     1.0,  1.0, 0.0,

     1.0,  1.0, 0.0,
     0.0,  0.0, 0.0,
     1.0,  0.0, 0.0,
};

float vs_tri[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

void twod_update_scr_dims(i32 w, i32 h)
{
    scr_w = w;
    scr_h = h;
}

void _map_pixel_ndc_dims(float* w, float* h)
{
    *w = (*w / scr_w) * 2.0;
    *h = (*h / scr_h) * 2.0;
}

void _map_pixel_ndc_coords(float* x, float* y)
{
    *x = (*x / scr_w) * 2.0f - 1.0f;
    *y = (*y / scr_h) * 2.0f - 1.0f;
}

void twod_init()
{
    /* vertex data */
    u32 vbo_rect;
    glGenVertexArrays(1, &vao_rect);
    glGenBuffers(1, &vbo_rect);

    glBindVertexArray(vao_rect);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_rect), vs_rect, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    /* shaders */
    shdr_simp = util_shader_load("shaders/twod_simp.vs", "shaders/twod_simp.fs");
    shdr_simp_loc_modl = glGetUniformLocation(shdr_simp, "modl");
    shdr_simp_loc_col = glGetUniformLocation(shdr_simp, "uni_frag_col");

    printf("twod_init...");
    printf("\n\tshdr_simp: %u", shdr_simp);
    printf("\n\tshdr_simp_loc_modl: %u", shdr_simp_loc_modl);
    printf("\n\tshdr_simp_loc_col: %u", shdr_simp_loc_col);
    printf("\n\tvao_rect: %u", vao_rect);
    printf("\n");
}

void twod_draw_rectf(float x, float y, float w, float h, Color c)
{
    _map_pixel_ndc_dims(&w, &h);
    _map_pixel_ndc_coords(&x, &y);

    mat4s modl = GLMS_MAT4_IDENTITY;
    /* modl = glms_scale(modl, {w, h, 1}); */
    /* modl = glms_translate(modl, {x, y, 0}); */

    vec3s vs[6];
    memcpy(vs, vs_rect, sizeof(float) * 18);
    for (int i = 0; i < 6; ++i) {
        vs[i].x *= w;
        vs[i].x += x;
        vs[i].y *= h;
        vs[i].y += y;
        vs[i].y *= -1;
        /* glms_vec3_print(vs[i], stdout); */
    }

    /* printf("twod_draw_rectf..\n"); */
    /* printf("w/h: %f/%f\tx/y: %f/%f\n", w * scr_w / 2.0f, h * scr_h / 2.0f, x, y); */
    /* glms_mat4_print(modl, stdout); */

    glUseProgram(shdr_simp);
    glBindVertexArray(vao_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_DYNAMIC_DRAW);
    glUniformMatrix4fv(shdr_simp_loc_modl, 1, GL_FALSE, (float*)&modl);
    glUniform4fv(shdr_simp_loc_col, 1, (float*)c.raw);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectv(vec2s a, float w, float h, Color c)
{
    twod_draw_rectf(a.x, a.y, w, h, c);
}










