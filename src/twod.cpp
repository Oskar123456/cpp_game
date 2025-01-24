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

#include <twod.hpp>
#include <logging.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <map>

using namespace std;

/* screen dims*/
static i32 scr_w;
static i32 scr_h;
/* text */
struct Glyph {
    u32 tex;
    vec2s sz;
    vec2s bearing;
    i64 adv;
};

static Glyph glyphs[256];
/* shaders */
static u32 shdr_simp;
static u32 shdr_simp_loc_col;
static u32 shdr_simp_loc_modl;

static u32 shdr_tex;
static u32 shdr_tex_loc_modl;

static u32 shdr_text;
static u32 shdr_text_loc_modl;
static u32 shdr_text_loc_col;

static u32 shdr_simp_circ;
static u32 shdr_simp_circ_loc_col;
static u32 shdr_simp_circ_loc_r;
static u32 shdr_simp_circ_loc_c;
static u32 shdr_simp_circ_loc_cr;
static u32 shdr_simp_circ_loc_modl;
static u32 shdr_simp_circ_loc_asp;
/* textures */
static map<string, u32> _twod_tex_map;
/* vertex data */
static u32 vao_rect;
static u32 vao_text;
static u32 vao_circ;

static u32 vbo_rect;
static u32 vbo_text;
static u32 vbo_circ;

static float vs_rect[] = {
/*    ndc-coords       tex-coords */
     -0.5, -0.5, -0.5, 0.0, 0.0,
     -0.5,  0.5, -0.5, 0.0, 1.0,
      0.5,  0.5, -0.5, 1.0, 1.0,

      0.5,  0.5, -0.5, 1.0, 1.0,
     -0.5, -0.5, -0.5, 0.0, 0.0,
      0.5, -0.5, -0.5, 1.0, 0.0,
};

static float vs_text[] = {
/*    ndc-coords       tex-coords */
     -0.5, -0.5, -0.5, 0.0, 1.0,
     -0.5,  0.5, -0.5, 0.0, 0.0,
      0.5,  0.5, -0.5, 1.0, 0.0,

      0.5,  0.5, -0.5, 1.0, 0.0,
     -0.5, -0.5, -0.5, 0.0, 1.0,
      0.5, -0.5, -0.5, 1.0, 1.0,
};

static float vs_circ[] = {
/*    ndc-coords       tex-coords */
     -1.0, -1.0, -1.0, 0.0, 0.0,
     -1.0,  1.0, -1.0, 0.0, 1.0,
      1.0,  1.0, -1.0, 1.0, 1.0,

      1.0,  1.0, -1.0, 1.0, 1.0,
     -1.0, -1.0, -1.0, 0.0, 0.0,
      1.0, -1.0, -1.0, 1.0, 0.0,
};

float vs_tri[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

/* text */

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
void _twod_transf_verts(float *vs, int vs_w, int vs_h, float x, float y, float w, float h)
{
    _twod_map_pixel_ndc_coords(&x, &y);
    _twod_map_pixel_ndc_dims(&w, &h);

    for (int i = 0; i < vs_h; ++i) {
        vs[i * vs_w]     = vs[i * vs_w] * w + x;
        vs[i * vs_w + 1] = vs[i * vs_w + 1] * h + y;
    }
}

void _twod_rot_rect(float *vs, int vs_w, int vs_h, float angle)
{
    for (int i = 0; i < vs_h; ++i) {
        vec3s v = {vs[i * vs_w], vs[i * vs_w + 1], vs[i * vs_w + 2]};
        v = vec3_rotate(v, angle, {0, 0, 1});
        vs[i * vs_w] = v.x;
        vs[i * vs_w + 1] = v.y;
        vs[i * vs_w + 2] = v.z;
    }
}

void twod_init()
{
    /* text */
    FT_Library ft;
    FT_Face ft_face;
    if (FT_Init_FreeType(&ft)) {
        LOG_ERROR("could not init freetype");
    }
    if (FT_New_Face(ft, "/home/oskar/Documents/LearnOpenGL/resources/fonts/Antonio-Bold.ttf", 0, &ft_face)) {
        LOG_ERROR("could not load face with freetype");
    }

    FT_Set_Pixel_Sizes(ft_face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (int i = 0; i < 256; ++i) {
        if (FT_Load_Char(ft_face, i, FT_LOAD_RENDER))
            continue;
        u32 tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE, ft_face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glyphs[i] = (Glyph){tex, {(float)ft_face->glyph->bitmap.width, (float)ft_face->glyph->bitmap.rows},
            {(float)ft_face->glyph->bitmap_left, (float)ft_face->glyph->bitmap_top},
            ft_face->glyph->advance.x};
    }
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft);
    /* vertex data */
    glGenVertexArrays(1, &vao_rect);
    glGenBuffers(1, &vbo_rect);

    glBindVertexArray(vao_rect);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_rect), vs_rect, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao_text);
    glGenBuffers(1, &vbo_text);

    glBindVertexArray(vao_text);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_text), vs_text, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao_circ);
    glGenBuffers(1, &vbo_circ);

    glBindVertexArray(vao_circ);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_circ);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_circ), vs_circ, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    /* shaders */
    shdr_simp = util_shader_load("shaders/twod_simp.vs", "shaders/twod_simp.fs");
    shdr_simp_loc_modl = glGetUniformLocation(shdr_simp, "modl");
    shdr_simp_loc_col = glGetUniformLocation(shdr_simp, "uni_frag_col");

    shdr_simp_circ = util_shader_load("shaders/twod_simp_circ.vs", "shaders/twod_simp_circ.fs");
    shdr_simp_circ_loc_modl = glGetUniformLocation(shdr_simp_circ, "modl");
    shdr_simp_circ_loc_r = glGetUniformLocation(shdr_simp_circ, "r");
    shdr_simp_circ_loc_c = glGetUniformLocation(shdr_simp_circ, "c");
    shdr_simp_circ_loc_cr = glGetUniformLocation(shdr_simp_circ, "cr");
    shdr_simp_circ_loc_asp = glGetUniformLocation(shdr_simp_circ, "asp");
    shdr_simp_circ_loc_col = glGetUniformLocation(shdr_simp_circ, "uni_frag_col");

    shdr_tex = util_shader_load("shaders/twod_tex.vs", "shaders/twod_tex.fs");
    shdr_tex_loc_modl = glGetUniformLocation(shdr_tex, "modl");

    shdr_text = util_shader_load("shaders/twod_text.vs", "shaders/twod_text.fs");
    shdr_text_loc_modl = glGetUniformLocation(shdr_text, "modl");
    shdr_text_loc_col = glGetUniformLocation(shdr_text, "uni_frag_col");
}

void twod_draw_circlef(float x, float y, float r, Color c)
{
    float rx = (r / scr_w) * 2.0f, ry = (r / scr_h) * 2.0f;
    float px = (x / scr_w) * 2.0f - 1.0f;
    float py = -1.0f * ((y / scr_h) * 2.0f - 1.0f);

    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, {px, py, 0});
    /* modl = glms_scale(modl, {rx * 2, ry * 2, 0}); */

    vec2s cp = {px, py};
    vec2s cr = {rx, ry};
    vec2s asp = {(float)scr_w, (float)scr_h};

    /* glms_mat4_print(modl, stdout); */

    glUseProgram(shdr_simp_circ);
    glBindVertexArray(vao_circ);
    glUniform4fv(shdr_simp_circ_loc_col, 1, (float*)c.raw);
    glUniform2fv(shdr_simp_circ_loc_c, 1, (float*)cp.raw);
    glUniform2fv(shdr_simp_circ_loc_cr, 1, (float*)cr.raw);
    glUniform2fv(shdr_simp_circ_loc_asp, 1, (float*)asp.raw);
    glUniform1f(shdr_simp_circ_loc_r, (rx) * (rx) + (ry) * (ry));
    glUniformMatrix4fv(shdr_simp_circ_loc_modl, 1, GL_FALSE, (float*)&modl);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectf(float x, float y, float w, float h, Color c)
{
    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, {((x / scr_w) * 2.0f - 1.0f), -1.0f * ((y / scr_h) * 2.0f - 1.0f), 0});
    modl = glms_scale(modl, {(w / scr_w) * 2.0f, (h / scr_h) * 2.0f, 1});

    glUseProgram(shdr_simp);
    glBindVertexArray(vao_rect);
    glUniform4fv(shdr_simp_loc_col, 1, (float*)c.raw);
    glUniformMatrix4fv(shdr_simp_loc_modl, 1, GL_FALSE, (float*)&modl);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectv(vec2s a, float w, float h, Color c)
{
    twod_draw_rectf(a.x, a.y, w, h, c);
}

void twod_draw_text(const char* txt, u32 txt_len, float x, float y, float sz, Color col, float angle)
{
    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, {((x / scr_w) * 2.0f - 1.0f), -1.0f * ((y / scr_h) * 2.0f - 1.0f), 0});
    modl = glms_scale(modl, {(glyphs['M'].sz.x / scr_w) * 2.0f, (glyphs['M'].sz.y / scr_h) * 2.0f, 1});
    modl = glms_rotate(modl, angle, {0, 0, 1});

    float scale = 340;
    float offs = 0;
    for (int i = 0; i < txt_len; ++i) {
        u8 c = txt[i];
        if (c < 0 || c > 255 || !glyphs[c].tex)
            continue;

        /* printf("rendering a %c (%d)\n", c, glyphs[c].tex); */

        offs = ((glyphs[c].bearing.x * scale) / scr_w) * 2.0f;
        modl = glms_translate(modl, {offs, 0, 0});
        /* modl = GLMS_MAT4_IDENTITY; */
        /* modl = glms_rotate(modl, angle, {0, 0, 1}); */

        /* glActiveTexture(GL_TEXTURE0); */
        glBindTexture(GL_TEXTURE_2D, glyphs[c].tex);
        /* glBindTexture(GL_TEXTURE_2D, _twod_tex_map["dirt"]); */
        glUseProgram(shdr_text);
        glBindVertexArray(vao_text);
        glUniformMatrix4fv(shdr_text_loc_modl, 1, GL_FALSE, (float*)&modl);
        glUniform4fv(shdr_text_loc_col, 1, (float*)col.raw);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void twod_draw_rectf_tex(float x, float y, float w, float h, Color c, const char* tex, float angle)
{
    if (!_twod_tex_map.count(tex)) {
        LOG_ERROR("could not find texture %s", tex);
        return;
    }

    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, {((x / scr_w) * 2.0f - 1.0f), -1.0f * ((y / scr_h) * 2.0f - 1.0f), 0});
    modl = glms_scale(modl, {(w / scr_w) * 2.0f, (h / scr_h) * 2.0f, 1});
    modl = glms_rotate(modl, angle, {0, 0, 1});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _twod_tex_map[tex]);

    glUseProgram(shdr_tex);
    glBindVertexArray(vao_rect);
    glUniformMatrix4fv(shdr_tex_loc_modl, 1, GL_FALSE, (float*)&modl);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectf_tex_rot(float x, float y, float w, float h, Color c, const char* tex, float angle)
{
    if (!_twod_tex_map.count(tex)) {
        LOG_ERROR("could not find texture %s", tex);
        return;
    }

    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, {((x / scr_w) * 2.0f - 1.0f), -1.0f * ((y / scr_h) * 2.0f - 1.0f), 0});
    modl = glms_scale(modl, {(w / scr_w) * 2.0f, (h / scr_h) * 2.0f, 1});
    modl = glms_rotate(modl, angle, {0, 0, 1});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _twod_tex_map[tex]);
    /* glBindTexture(GL_TEXTURE_2D, glyphs['O'].tex); */

    glUseProgram(shdr_tex);
    glBindVertexArray(vao_rect);
    glUniformMatrix4fv(shdr_tex_loc_modl, 1, GL_FALSE, (float*)&modl);
    /* glUniform4fv(shdr_text_loc_col, 1, (float*)c.raw); */
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

u32 twod_create_tex(const char* img_path, const char* alias)
{
    u32 tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, n_chans;
    stbi_set_flip_vertically_on_load(true);
    u8* img = stbi_load(img_path, &w, &h, &n_chans, 0);

    if (img)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_ERROR("err loading texture %s", img_path);
        return 0;
    }

    stbi_image_free(img);
    _twod_tex_map[alias] = tex;
    return tex;
}

u32 twod_create_tex_a(const char* img_path, const char* alias)
{
    u32 tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, n_chans;
    stbi_set_flip_vertically_on_load(true);
    u8* img = stbi_load(img_path, &w, &h, &n_chans, 0);

    if (img)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_ERROR("err loading texture %s", img_path);
        return 0;
    }

    stbi_image_free(img);
    _twod_tex_map[alias] = tex;
    return tex;
}



