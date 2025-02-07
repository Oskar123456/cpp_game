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
#include <threed.h>
#include <logging.h>
/* #include <SDL3/SDL.h> */
#include <glad/glad.h>

/* #include <ft2build.h> */
/* #include FT_FREETYPE_H */
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

using namespace std;

static u32 vao_cube;
static u32 shader_cube;
static u32 shader_cube_mvp;
static u32 shader_cube_col;

static float vs_cube[] = {
/*  POSITION            |TEX COORD   |NORMAL           */
/*  front                                              */
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
/*  back                                               */
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
/*  left                                               */
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
/*  right                                              */
     0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
/*  top                                                */
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
/*  bottom                                             */
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
};

static u32 eab_cube[] = { 0, 1, 2, 0, 2, 3 };

void threed_render_cube(vec2s& pos, mat4s& rot, Color col)
{
}

void threed_init()
{
    /* text */
    /* FT_Library ft; */
    /* FT_Face ft_face; */
    /* if (FT_Init_FreeType(&ft)) { */
    /*     LOG_ERROR("could not init freetype"); */
    /* } */
    /* if (FT_New_Face(ft, "/home/oskar/.fonts/overpass/overpass/overpass-regular.otf", 0, &ft_face)) { */
    /*     LOG_ERROR("could not load face with freetype"); */
    /* } */

    /* FT_Set_Pixel_Sizes(ft_face, 0, 48); */
    /* glPixelStorei(GL_UNPACK_ALIGNMENT, 1); */
    /* for (int i = 0; i < 256; ++i) { */
    /*     if (FT_Load_Char(ft_face, i, FT_LOAD_RENDER)) */
    /*         continue; */
    /*     u32 tex; */
    /*     glGenTextures(1, &tex); */
    /*     glBindTexture(GL_TEXTURE_2D, tex); */
    /*     glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, */
    /*             ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows, */
    /*             0, GL_RED, GL_UNSIGNED_BYTE, ft_face->glyph->bitmap.buffer); */
    /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); */
    /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); */
    /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
    /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
    /*     glyphs[i] = (Glyph){tex, {(float)ft_face->glyph->bitmap.width, (float)ft_face->glyph->bitmap.rows}, */
    /*         {(float)ft_face->glyph->bitmap_left, (float)ft_face->glyph->bitmap_top}, */
    /*         ft_face->glyph->advance.x}; */
    /* } */
    /* FT_Done_Face(ft_face); */
    /* FT_Done_FreeType(ft); */

    /* vertex data */
    u32 vbo_cube;
    glGenVertexArrays(1, &vao_cube);
    glGenBuffers(1, &vbo_cube);
    glBindVertexArray(vao_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_cube), vs_cube, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eab_cube), eab_cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)5);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    /* shaders */
    /* shdr_simp = util_shader_load("shaders/twod_simp.vs", "shaders/twod_simp.fs"); */
    /* shdr_simp_loc_mvp = glGetUniformLocation(shdr_simp, "mvp"); */
    /* shdr_simp_loc_col = glGetUniformLocation(shdr_simp, "color"); */
}








































