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

#include <string>
#include <render.h>
#include <glad/glad.h>

struct Light_Shader_Locs {
    u32 ambient, diffuse, specular;
    u32 position, direction;
    u32 attenuation;
};

struct Material_Shader_Locs {
    u32 ambient, diffuse, specular;
    u32 smoothness;
};

using namespace std;

static u32 shader_default3D;
static u32 shader_default3D_modl;
static u32 shader_default3D_view;
static u32 shader_default3D_proj;
static u32 shader_default3D_normal_matrix;
static u32 shader_default3D_tint;
static u32 shader_default3D_use_tex;
static u32 shader_default3D_lights_num;

static int shader_default3D_lights_index;
static int shader_default3D_lights_count;
static const int shader_default3D_lights_max = 10;
static Light_Shader_Locs shader_default3D_lights[shader_default3D_lights_max];
static Material_Shader_Locs shader_default3D_material;

void render_init()
{
    shader_default3D = util_shader_load("shaders/default3D.vs", "shaders/default3D.fs");
    shader_default3D_modl = glGetUniformLocation(shader_default3D, "modl");
    shader_default3D_view = glGetUniformLocation(shader_default3D, "view");
    shader_default3D_proj = glGetUniformLocation(shader_default3D, "proj");
    shader_default3D_normal_matrix = glGetUniformLocation(shader_default3D, "normal_matrix");
    shader_default3D_tint = glGetUniformLocation(shader_default3D, "tint");
    shader_default3D_use_tex = glGetUniformLocation(shader_default3D, "use_tex");
    shader_default3D_lights_num = glGetUniformLocation(shader_default3D, "lights_num");

    shader_default3D_material.ambient = glGetUniformLocation(shader_default3D, "material.ambient");
    shader_default3D_material.diffuse = glGetUniformLocation(shader_default3D, "material.diffuse");
    shader_default3D_material.specular = glGetUniformLocation(shader_default3D, "material.specular");
    shader_default3D_material.smoothness = glGetUniformLocation(shader_default3D, "material.smoothness");

    for (int i = 0; i < shader_default3D_lights_max; ++i) {
        string ambi = "lights[" + to_string(i) + "].ambient";
        string diff = "lights[" + to_string(i) + "].diffuse";
        string spec = "lights[" + to_string(i) + "].specular";
        string pos  = "lights[" + to_string(i) + "].position";
        string dir  = "lights[" + to_string(i) + "].direction";
        string att  = "lights[" + to_string(i) + "].attenuation";
        shader_default3D_lights[i].ambient = glGetUniformLocation(shader_default3D, ambi.c_str());
        shader_default3D_lights[i].diffuse = glGetUniformLocation(shader_default3D, diff.c_str());
        shader_default3D_lights[i].specular = glGetUniformLocation(shader_default3D, spec.c_str());
        shader_default3D_lights[i].position = glGetUniformLocation(shader_default3D, pos.c_str());
        shader_default3D_lights[i].direction = glGetUniformLocation(shader_default3D, dir.c_str());
        shader_default3D_lights[i].attenuation = glGetUniformLocation(shader_default3D, att.c_str());
        printf("%d, %d, %d, %d, %d, %d\n",
                shader_default3D_lights[i].ambient,
                shader_default3D_lights[i].diffuse,
                shader_default3D_lights[i].specular,
                shader_default3D_lights[i].position,
                shader_default3D_lights[i].direction,
                shader_default3D_lights[i].attenuation);
    }

    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR) {
        printf("%d\n", e);
    }

    printf("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", shader_default3D, shader_default3D_modl,
            shader_default3D_view, shader_default3D_proj, shader_default3D_normal_matrix,
            shader_default3D_material.ambient, shader_default3D_material.diffuse,
            shader_default3D_material.specular, shader_default3D_material.smoothness);
}

void render_add_lights(Light* lights, int n)
{
    glUseProgram(shader_default3D);

    for (int i = 0; i < n; ++i) {
        Light_Shader_Locs& lhs = shader_default3D_lights[(i + shader_default3D_lights_index) % shader_default3D_lights_max];
        glUniform3fv(lhs.ambient, 1, lights[i].ambient.raw);
        glUniform3fv(lhs.diffuse, 1, lights[i].diffuse.raw);
        glUniform3fv(lhs.specular, 1, lights[i].specular.raw);
        glUniform3fv(lhs.position, 1, lights[i].position.raw);
        glUniform3fv(lhs.direction, 1, lights[i].direction.raw);
        glUniform1f(lhs.attenuation, lights[i].attenuation);
        shader_default3D_lights_index++;
    }

    shader_default3D_lights_count = min(shader_default3D_lights_count + n, shader_default3D_lights_max);
    glUniform1i(shader_default3D_lights_num, shader_default3D_lights_count);
}

void render(Camera& cam, Model& model, vec3s& pos, vec3s& scale, float angle, vec3s& axis, vec4s& tint, bool use_wireframe)
{
    if (use_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, pos);
    modl = glms_scale(modl, scale);
    modl = glms_rotate(modl, angle, axis);

    // glms_mat4_print(modl, stdout);

    glUseProgram(shader_default3D);
    glBindTexture(GL_TEXTURE_2D, model.texture);
    glBindVertexArray(model.mesh);

    // printf("%d\n", model.texture);

    glUniformMatrix4fv(shader_default3D_modl, 1, GL_FALSE, (float*)&modl);
    glUniformMatrix4fv(shader_default3D_view, 1, GL_FALSE, (float*)&cam.view);
    glUniformMatrix4fv(shader_default3D_proj, 1, GL_FALSE, (float*)&cam.proj);

    glUniform3fv(shader_default3D_material.ambient, 1, model.material.ambient.raw);
    glUniform3fv(shader_default3D_material.diffuse, 1, model.material.diffuse.raw);
    glUniform3fv(shader_default3D_material.specular, 1, model.material.specular.raw);
    glUniform1f(shader_default3D_material.smoothness, model.material.smoothness);

    glUniform4fv(shader_default3D_tint, 1, (float*)&tint);
    glUniform1i(shader_default3D_use_tex, model.texture > 0);

    glDrawElements(GL_TRIANGLES, model.triangles, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (use_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}




























