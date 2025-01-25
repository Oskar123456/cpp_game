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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <vector>
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
static u32 shdr_text_loc_proj;
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
    if (FT_New_Face(ft, "/usr/share/fonts/opentype/fira/FiraSans-Regular.otf", 0, &ft_face)) {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_text), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
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
    shdr_text_loc_proj = glGetUniformLocation(shdr_text, "proj");
    shdr_text_loc_col = glGetUniformLocation(shdr_text, "text_color");
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

void twod_draw_text(const char* txt, u32 txt_len, float x, float y, float scale, Color col, float angle)
{
    y = scr_h - y;
    float x_o = x, y_o = y;

    glUseProgram(shdr_text);
    mat4s proj = glms_ortho(0, scr_w, 0, scr_h, 0.00, 100);
    /* proj = glms_rotate(proj, angle, {0, 0, 1}); */
    glUniformMatrix4fv(shdr_text_loc_proj, 1, GL_FALSE, (float*)&proj);
    glUniform4fv(shdr_text_loc_col, 1, (float*)&col);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao_text);

    for (int i = 0; i < txt_len; ++i) {
        u8 c = txt[i];
        if (c < 0 || c > 255 || !glyphs[c].tex)
            glBindTexture(GL_TEXTURE_2D, glyphs['?'].tex);
        else
            glBindTexture(GL_TEXTURE_2D, glyphs[c].tex);

        float x_off = x + glyphs[c].bearing.x * scale;
        float y_off = y - (glyphs[c].sz.y - glyphs[c].bearing.y) * scale;
        float w = glyphs[c].sz.x * scale;
        float h = glyphs[c].sz.y * scale;

        float vs[6][4] = {
            { x_off,     y_off + h,   0.0f, 0.0f },
            { x_off,     y_off,       0.0f, 1.0f },
            { x_off + w, y_off,       1.0f, 1.0f },

            { x_off,     y_off + h,   0.0f, 0.0f },
            { x_off + w, y_off,       1.0f, 1.0f },
            { x_off + w, y_off + h,   1.0f, 0.0f }
        };

        for (int i = 0; i < 6; ++i) {
            vec2s v = {vs[i][0], vs[i][1]};
            v = vec2_sub(v, {x_o, y_o});
            v = vec2_rotate(v, angle);
            v = vec2_add(v, {x_o, y_o});
            vs[i][0] = v.x;
            vs[i][1] = v.y;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vs), vs);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (glyphs[c].adv >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

float twod_get_text_length(const char* txt, u32 txt_len, float scale)
{
    float len = 0;
    for (int i = 0; i < txt_len; ++i) {
        u8 c = txt[i];
        if (c > 255 || !glyphs[c].tex)
            c = '?';
        len += (glyphs[c].adv >> 6) * scale;
    }
    return len;
}

void screenshot()
{
    time_t t;
    struct tm t_s;
    vector<char> buffer;

    time(&t);
    localtime_r(&t, &t_s);

    char t_str[strlen("screenshot_00-00-0000" + 1)] = "screenshot_";
    strftime(&t_str[strlen("screenshot_")], strlen("00-00-0000") + 1, "%d-%m-%Y", &t_s);

    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * scr_w;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei buf_sz = stride * scr_h;
    buffer.reserve(buf_sz);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, scr_w, scr_h, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

    stbi_flip_vertically_on_write(true);
    stbi_write_png(t_str, scr_w, scr_h, nrChannels, buffer.data(), stride);

    printf("saved screenshot to %s/%s\n", getenv("PWD"), t_str);
}
