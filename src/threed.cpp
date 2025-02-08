/*=======================================
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
#include <glad/glad.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <map>
#include <string>
#include <cstring>

using namespace std;

static map<string, u32> textures;

static u32 vao_cube;
static u32 shader_cube;
static u32 shader_cube_mvp;
static u32 shader_cube_col;
static u32 shader_cube_use_tex;

static float vs_cube[] = {
/*  POSITION            |TEX COORD   |NORMAL           */
/*  top                                                */
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
/*  bottom                                             */
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
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
};

static u32 eab_cube[] = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
};

void threed_render_cube(mat4s& view_proj, vec3s& pos, float angle, vec3s rot_axis, Color col, u32 vao, u32 tex_id)
{
    mat4s mvp = GLMS_MAT4_IDENTITY;
    mvp = mat4_mul(mvp, view_proj);
    mvp = glms_translate(mvp, pos);
    mvp = glms_rotate(mvp, angle, rot_axis);

    /* vec3s p = mat4_mulv3(mvp, pos, 1); */

    /* glms_vec3_print(p, stdout); */

    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

    glUseProgram(shader_cube);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glBindVertexArray((vao > 0) ? vao : vao_cube);
    glUniformMatrix4fv(shader_cube_mvp, 1, GL_FALSE, (float*)&mvp);
    glUniform4fv(shader_cube_col, 1, (float*)&col);
    glUniform1i(shader_cube_use_tex, tex_id > 0);
    glDrawElements(GL_TRIANGLES, sizeof(eab_cube), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

u32 threed_create_tex(const char* img_path, const char* alias, int format)
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

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        LOG_ERROR("err loading texture %s", img_path);
        return 0;
    }

    if (textures.count(alias)) {
        LOG_WARN("texture %s exists, overwriting...", alias);
    }

    stbi_image_free(img);
    textures[alias] = tex;
    return tex;
}

void threed_init()
{
    /* text */
    /* vertex data */
    u32 vbo_cube, ebo_cube;
    glGenVertexArrays(1, &vao_cube);
    glGenBuffers(1, &vbo_cube);
    glGenBuffers(1, &ebo_cube);

    glBindVertexArray(vao_cube);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs_cube), vs_cube, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eab_cube), eab_cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    /* shaders */
    shader_cube = util_shader_load("shaders/threed_cube.vs", "shaders/threed_cube.fs");
    shader_cube_mvp = glGetUniformLocation(shader_cube, "mvp");
    shader_cube_col = glGetUniformLocation(shader_cube, "color");
    shader_cube_use_tex = glGetUniformLocation(shader_cube, "use_tex");
}

u32 threed_create_cube_vao(vec2s (&tex_coords)[24])
{
    float vs[sizeof(vs_cube)];
    memcpy(vs, vs_cube, sizeof(vs_cube));

    for (int i = 0; i < 24; ++i) {
        vs[i * 8 + 3] = tex_coords[i].x;
        vs[i * 8 + 4] = tex_coords[i].y;
    }

    u32 vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eab_cube), eab_cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return vao;
}






































