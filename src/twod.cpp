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

#include <unistd.h>

/* screen dims*/
static i32 scr_w;
static i32 scr_h;
/* shaders */
static u32 shdr_simp;
static u32 shdr_simp_loc_col;
static u32 shdr_simp_loc_modl;
/* vertex data */
static u32 vao_rect;
static u32 vao_circ;

u32 vbo_rect;
u32 vbo_circ;
/* text */
TTF_Font *font_default;

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

static float vs_circ[] = {
     0.0,  0.0, 0.0,
     0.0,  1.0, 0.0,
     1.0,  1.0, 0.0,

     1.0,  1.0, 0.0,
     0.0,  0.0, 0.0,
     1.0,  0.0, 0.0,
};

void twod_update_scr_dims(i32 w, i32 h)
{
    scr_w = w;
    scr_h = h;
}

void _twod_print_vec3(vec3s v)
{
    printf("[%.3f,%.3f,%.3f]", v.x, v.y, v.z);
}

void _twod_map_pixel_ndc_dims(float* w, float* h)
{
    *w = (*w / scr_w) * 2.0;
    *h = (*h / scr_h) * 2.0;
}

void _twod_map_pixel_ndc_coords(float* x, float* y)
{
    *x = (*x / scr_w) * 2.0f - 1.0f;
    *y = (-1.0f) * ((*y / scr_h) * 2.0f - 1.0f);
}
/**
 * Coordinates and scale in screen pixels.
 */
void _twod_transf_verts(vec3s *vs, int sz, float x, float y, float w, float h)
{
    _twod_map_pixel_ndc_coords(&x, &y);
    _twod_map_pixel_ndc_dims(&w, &h);

    for (int i = 0; i < sz; ++i) {
        vs[i].x = vs[i].x * w + x;
        vs[i].y = (vs[i].y * h + y);
        /* glms_vec3_print(vs[i], stdout); */
    }
}

void twod_init()
{
    /* vertex data */
    glGenVertexArrays(1, &vao_rect);
    glGenBuffers(1, &vbo_rect);

    glBindVertexArray(vao_rect);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_rect), vs_rect, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao_circ);
    glGenBuffers(1, &vbo_circ);

    glBindVertexArray(vao_circ);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_circ);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_circ), vs_circ, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    /* shaders */
    shdr_simp = util_shader_load("shaders/twod_simp.vs", "shaders/twod_simp.fs");
    shdr_simp_loc_modl = glGetUniformLocation(shdr_simp, "modl");
    shdr_simp_loc_col = glGetUniformLocation(shdr_simp, "uni_frag_col");

    /* text */
    /* if (!TTF_Init()) { */
    /*     printf("could not init TTF_Font: %s\n", SDL_GetError()); */
    /* } */
    font_default = TTF_OpenFont("/home/oskar/.fonts/FiraCode/FiraMonoNerdFont-Medium.otf", 20);
    if (!font_default) {
        printf("could not open default font: %s\n", SDL_GetError());
    }
}

void twod_draw_circlef(float x, float y, float r, Color c)
{
    int nvs = 100;
    vec3s vs[nvs + 1];

    float rx = r, ry = r;
    _twod_map_pixel_ndc_coords(&x, &y);
    _twod_map_pixel_ndc_dims(&rx, &ry);

    vs[0] = {x, y, 0};
    for (int i = 0; i <= nvs; ++i) {
        vs[i].x = x + cos(((M_PI * 2.0f) / nvs) * (i % nvs)) * rx;
        vs[i].y = y + sin(((M_PI * 2.0f) / nvs) * (i % nvs)) * ry;
        vs[i].z = 0;
    }

    mat4s modl = GLMS_MAT4_IDENTITY;

    glUseProgram(shdr_simp);
    glBindVertexArray(vao_circ);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_circ);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_DYNAMIC_DRAW);
    glUniform4fv(shdr_simp_loc_col, 1, (float*)c.raw);
    glUniformMatrix4fv(shdr_simp_loc_modl, 1, GL_FALSE, (float*)&modl);
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(vs));
    glBindVertexArray(0);
}

void twod_draw_rectf(float x, float y, float w, float h, Color c)
{
    vec3s vs[6];
    memcpy(vs, vs_rect, sizeof(vec3s) * 6);
    _twod_transf_verts(vs, 6, x, y, w, h);

    mat4s modl = GLMS_MAT4_IDENTITY;

    glUseProgram(shdr_simp);
    glBindVertexArray(vao_rect);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_DYNAMIC_DRAW);
    glUniform4fv(shdr_simp_loc_col, 1, (float*)c.raw);
    glUniformMatrix4fv(shdr_simp_loc_modl, 1, GL_FALSE, (float*)&modl);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectv(vec2s a, float w, float h, Color c)
{
    twod_draw_rectf(a.x, a.y, w, h, c);
}

void twod_draw_text(float x, float y, float sz, Color c)
{

}








