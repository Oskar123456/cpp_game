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

#include <twod.h>
#include <logging.h>

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
static u32 shdr_simp_loc_mvp;

static u32 shdr_rounded_corners;
static u32 shdr_rounded_corners_loc_mvp;
static u32 shdr_rounded_corners_loc_modl;
static u32 shdr_rounded_corners_loc_col;
static u32 shdr_rounded_corners_loc_use_tex;
static u32 shdr_rounded_corners_loc_dims;
static u32 shdr_rounded_corners_loc_center;
static u32 shdr_rounded_corners_loc_border_rad;

static u32 shdr_tex;
static u32 shdr_tex_loc_mvp;
static u32 shdr_tex_loc_col;

static u32 shdr_text;
static u32 shdr_text_loc_proj;
static u32 shdr_text_loc_col;

static u32 shdr_simp_circ;
static u32 shdr_simp_circ_loc_col;
static u32 shdr_simp_circ_loc_proj;
static u32 shdr_simp_circ_loc_r;
static u32 shdr_simp_circ_loc_c;
static u32 shdr_simp_circ_loc_use_tex;
/* textures */
static map<string, u32> _twod_tex_map;
/* vertex data */
static u32 vao_rect;
static u32 vao_tex;
static u32 vao_text;
static u32 vao_circ;

static u32 vbo_rect;
static u32 vbo_tex;
static u32 vbo_text;
static u32 vbo_circ;

static float vs_rect[] = {
/*    ndc-coords  tex-coords */
     -0.5, -0.5,  0.0, 0.0,
     -0.5,  0.5,  0.0, 1.0,
      0.5,  0.5,  1.0, 1.0,

      0.5,  0.5,  1.0, 1.0,
     -0.5, -0.5,  0.0, 0.0,
      0.5, -0.5,  1.0, 0.0,
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

static float vs_circ[6][4];

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
    if (FT_New_Face(ft, "/home/oskar/.fonts/lexica-ultra/lexica-ultralegible-release/fonts/ttf/LexicaUltralegible-Regular.ttf", 0, &ft_face)) {
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao_tex);
    glGenBuffers(1, &vbo_tex);

    glBindVertexArray(vao_tex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_rect), vs_rect, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_circ), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    /* shaders */
    shdr_simp = util_shader_load("shaders/twod_simp.vs", "shaders/twod_simp.fs");
    shdr_simp_loc_mvp = glGetUniformLocation(shdr_simp, "mvp");
    shdr_simp_loc_col = glGetUniformLocation(shdr_simp, "color");

    shdr_rounded_corners = util_shader_load("shaders/twod_rounded_corners.vs", "shaders/twod_rounded_corners.fs");
    shdr_rounded_corners_loc_mvp = glGetUniformLocation(shdr_rounded_corners, "mvp");
    shdr_rounded_corners_loc_modl = glGetUniformLocation(shdr_rounded_corners, "modl");
    shdr_rounded_corners_loc_col = glGetUniformLocation(shdr_rounded_corners, "color");
    shdr_rounded_corners_loc_use_tex = glGetUniformLocation(shdr_rounded_corners, "use_tex");
    shdr_rounded_corners_loc_dims = glGetUniformLocation(shdr_rounded_corners, "dims");
    shdr_rounded_corners_loc_center = glGetUniformLocation(shdr_rounded_corners, "center");
    shdr_rounded_corners_loc_border_rad = glGetUniformLocation(shdr_rounded_corners, "border_rad");

    shdr_simp_circ = util_shader_load("shaders/twod_simp_circ.vs", "shaders/twod_simp_circ.fs");
    shdr_simp_circ_loc_col = glGetUniformLocation(shdr_simp_circ, "color");
    shdr_simp_circ_loc_proj = glGetUniformLocation(shdr_simp_circ, "proj");
    shdr_simp_circ_loc_r = glGetUniformLocation(shdr_simp_circ, "r");
    shdr_simp_circ_loc_c = glGetUniformLocation(shdr_simp_circ, "c");
    shdr_simp_circ_loc_use_tex = glGetUniformLocation(shdr_simp_circ, "use_tex");

    shdr_tex = util_shader_load("shaders/twod_tex.vs", "shaders/twod_tex.fs");
    shdr_tex_loc_mvp = glGetUniformLocation(shdr_tex, "mvp");
    shdr_tex_loc_col = glGetUniformLocation(shdr_tex, "color");

    shdr_text = util_shader_load("shaders/twod_text.vs", "shaders/twod_text.fs");
    shdr_text_loc_proj = glGetUniformLocation(shdr_text, "proj");
    shdr_text_loc_col = glGetUniformLocation(shdr_text, "text_color");
}

void twod_draw_circlef(float x, float y, float r, Color col)
{
    mat4s proj = glms_ortho(0, scr_w, 0, scr_h, 0, 1);
    vec2s c = {x, y};

    glUseProgram(shdr_simp_circ);
    glBindVertexArray(vao_circ);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_circ);

    float vs[6][4] = {
        { x - r * r,  y + r * r,  0.0f, 0.0f },
        { x - r * r,  y - r * r,  0.0f, 1.0f },
        { x + r  * r, y - r * r,  1.0f, 1.0f },

        { x - r * r,  y + r  * r, 0.0f, 0.0f },
        { x + r  * r, y - r * r,  1.0f, 1.0f },
        { x + r  * r, y + r  * r, 1.0f, 0.0f }
    };

    /* for (int i = 0; i < 6; ++i) { */
    /*     printf("%.1f %.1f %.1f\n", vs[i][0], vs[i][1], vs[i][2]); */
    /* } */

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vs), vs);

    glUniform4fv(shdr_simp_circ_loc_col, 1, (float*)col.raw);
    glUniformMatrix4fv(shdr_simp_circ_loc_proj, 1, GL_FALSE, (float*)proj.raw);
    glUniform2fv(shdr_simp_circ_loc_c, 1, (float*)c.raw);
    glUniform1f(shdr_simp_circ_loc_r, r);
    glUniform1i(shdr_simp_circ_loc_use_tex, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void twod_draw_rectf(float x, float y, float w, float h, Color col, float angle)
{
    y = scr_h - y;

    mat4s proj = glms_ortho(0, scr_w, 0, scr_h, 0, 1);
    mat4s modl = glms_translate(GLMS_MAT4_IDENTITY, {x + 0.5f, y + 0.5f, 0});
    modl = glms_scale(modl, {w, h, 1});
    modl = glms_rotate(modl, angle, {0, 0, 1});
    mat4s mvp = mat4_mul(proj, modl);

    glUseProgram(shdr_simp);
    glBindVertexArray(vao_rect);
    glUniform4fv(shdr_simp_loc_col, 1, (float*)col.raw);
    glUniformMatrix4fv(shdr_simp_loc_mvp, 1, GL_FALSE, (float*)&mvp);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectf_tex(float x, float y, float w, float h, Color col, const char* tex, float angle)
{
    y = scr_h - y;

    if (!_twod_tex_map.count(tex)) {
        LOG_ERROR("could not find texture %s", tex);
        return;
    }

    mat4s proj = glms_ortho(0, scr_w, 0, scr_h, 0, 1);
    mat4s modl = glms_translate(GLMS_MAT4_IDENTITY, {x + 0.5f, y + 0.5f, 0});
    modl = glms_scale(modl, {w, h, 1});
    modl = glms_rotate(modl, angle, {0, 0, 1});
    mat4s mvp = mat4_mul(proj, modl);

    glUseProgram(shdr_tex);
    glBindVertexArray(vao_tex);
    glBindTexture(GL_TEXTURE_2D, _twod_tex_map[tex]);
    glUniform4fv(shdr_tex_loc_col, 1, (float*)col.raw);
    glUniformMatrix4fv(shdr_tex_loc_mvp, 1, GL_FALSE, (float*)&mvp);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_rectv(vec2s a, float w, float h, Color c)
{
    twod_draw_rectf(a.x, a.y, w, h, c, 0);
}

/**
 * @param[in] tex set to NULL if none wanted, and color to (1, 1, 1, 1) if no tint wanted
 */
void twod_draw_rectf_rounded(float x, float y, float w, float h, float border_radius, Color col, const char* tex, float angle)
{
    y = scr_h - y;

    if (tex && !_twod_tex_map.count(tex)) {
        LOG_ERROR("could not find texture %s", tex);
        return;
    }

    vec2s dims = {w, h};
    vec2s center = {x, y};

    mat4s modl_uni = glms_translate(GLMS_MAT4_IDENTITY, {x + 0.5f, y + 0.5f, 0});
    modl_uni = glms_scale(modl_uni, {w, h, 1});

    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, {x + 0.5f, y + 0.5f, 0});
    modl = glms_scale(modl, {w, h, 1});
    modl = glms_rotate(modl, angle, {0, 0, 1});

    mat4s proj = glms_ortho(0, scr_w, 0, scr_h, 0, 1);

    mat4s mvp = mat4_mul(proj, modl);

    glUseProgram(shdr_rounded_corners);
    glBindVertexArray(vao_tex);
    if (tex)
        glBindTexture(GL_TEXTURE_2D, _twod_tex_map[tex]);

    glUniform1i(shdr_rounded_corners_loc_use_tex, !(tex == NULL));
    glUniform1f(shdr_rounded_corners_loc_border_rad, border_radius);
    glUniform2fv(shdr_rounded_corners_loc_dims, 1, (float*)&dims);
    glUniform2fv(shdr_rounded_corners_loc_center, 1, (float*)&center);
    glUniform4fv(shdr_rounded_corners_loc_col, 1, (float*)&col);
    glUniformMatrix4fv(shdr_rounded_corners_loc_mvp, 1, GL_FALSE, (float*)&mvp);
    glUniformMatrix4fv(shdr_rounded_corners_loc_modl, 1, GL_FALSE, (float*)&modl_uni);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void twod_draw_text(const char* txt, u32 txt_len, float x, float y, float scale, Color col, float angle)
{
    y = scr_h - y;
    float x_o = x, y_o = y;

    glUseProgram(shdr_text);
    mat4s proj = glms_ortho(0, scr_w, 0, scr_h, 0, 1);
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

    char t_str[strlen("screenshot_00-00-0000") + 1] = "screenshot_";
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
