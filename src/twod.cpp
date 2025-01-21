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

/* shaders */
static u32 shdr_simp;
static u32 shdr_simp_loc_col;
/* vertex data */
static u32 vao_rect;

static float vs_rect[] = {
    -1.0, -1.0, 0.0,
    -1.0,  1.0, 0.0,
     1.0,  1.0, 0.0,

     1.0,  1.0, 0.0,
    -1.0, -1.0, 0.0,
     1.0, -1.0, 0.0,
};

void twod_init()
{
    /* vertex data */
    u32 vbo_rect;
    glGenVertexArrays(1, &vao_rect);
    glGenBuffers(1, &vbo_rect);

    glBindVertexArray(vao_rect);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_rect), vs_rect, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 6, GL_FLOAT, GL_FALSE, 6, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    /* shaders */
    shdr_simp = util_shader_load("shaders/twod_simp.vs", "shaders/twod_simp.fs");
    shdr_simp_loc_col = glGetUniformLocation(shdr_simp, "uni_frag_col");

    printf("twod_init...");
    printf("\n\tshdr_simp: %u", shdr_simp);
    printf("\n\tvao_rect: %u", vao_rect);
    printf("\n");
}

void twod_draw_rectf(float ax, float ay, float bx, float by, Color c)
{
    glUseProgram(shdr_simp);
    glBindVertexArray(vao_rect);
    glUniform4fv(shdr_simp_loc_col, 1, (float*)c.raw);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectv(vec2s a, vec2s b, Color c)
{
    twod_draw_rectf(a.x, a.y, b.x, b.y, c);
}










