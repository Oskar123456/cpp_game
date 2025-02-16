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

#define DIR_LIGHT_MAX 1
#define POINT_LIGHT_MAX 20
#define SPOT_LIGHT_MAX 20

struct Light_Shader_Locs {
    i32 ambient, diffuse, specular;
    i32 position, direction;
    i32 attenuation;
};

struct Shader_Material {
    i32 ambient, diffuse, specular;
    i32 shininess, smoothness;
};

struct Shader_Dir_Light {
    i32 direction;
    i32 ambient, diffuse, specular;
};

struct Shader_Point_Light {
    i32 position;
    i32 constant, linear, quadratic;
    i32 ambient, diffuse, specular;
};

struct Shader_Spot_Light {
    i32 position, direction;
    i32 cutoff, cutoff_outer;
    i32 constant, linear, quadratic;
    i32 ambient, diffuse, specular;
};

struct Shader {
    i32 id;
    i32 eye, modl, view, proj;
    i32 tint, use_tex;

    Shader_Material material;
    Shader_Dir_Light dir_lights[DIR_LIGHT_MAX];
    Shader_Point_Light point_lights[POINT_LIGHT_MAX];
    Shader_Spot_Light spot_lights[SPOT_LIGHT_MAX];
    i32 dir_light_idx, point_light_idx, spot_light_idx;
};

using namespace std;

static Shader shader_default3D;

Shader& render_get_shader_default()
{
    return shader_default3D;
}

void render_clear_dir_lights()
{
    Dir_Light dirlight_init = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    for (int i = 0; i < DIR_LIGHT_MAX; ++i)
        render_add_dir_light(shader_default3D, dirlight_init);
}

void render_clear_point_lights()
{
    Point_Light pointlight_init = { {0, 0, 0}, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    for (int i = 0; i < DIR_LIGHT_MAX; ++i)
        render_add_point_light(shader_default3D, pointlight_init);
}

void render_clear_spot_lights()
{
    Spot_Light spotlight_init = { {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    for (int i = 0; i < DIR_LIGHT_MAX; ++i)
        render_add_spot_light(shader_default3D, spotlight_init);
}

void render_init()
{
    shader_default3D.id = util_shader_load("shaders/default3D.vs", "shaders/default3D.fs");
    shader_default3D.eye = glGetUniformLocation(shader_default3D.id, "eye");
    shader_default3D.modl = glGetUniformLocation(shader_default3D.id, "modl");
    shader_default3D.view = glGetUniformLocation(shader_default3D.id, "view");
    shader_default3D.proj = glGetUniformLocation(shader_default3D.id, "proj");
    shader_default3D.tint = glGetUniformLocation(shader_default3D.id, "tint");
    shader_default3D.use_tex = glGetUniformLocation(shader_default3D.id, "use_tex");

    shader_default3D.material.ambient = glGetUniformLocation(shader_default3D.id, "material.ambient");
    shader_default3D.material.diffuse = glGetUniformLocation(shader_default3D.id, "material.diffuse");
    shader_default3D.material.specular = glGetUniformLocation(shader_default3D.id, "material.specular");
    shader_default3D.material.shininess = glGetUniformLocation(shader_default3D.id, "material.shininess");
    shader_default3D.material.smoothness = glGetUniformLocation(shader_default3D.id, "material.smoothness");

    Dir_Light dirlight_init = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    Point_Light pointlight_init = { {0, 0, 0}, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    Spot_Light spotlight_init = { {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };

    for (int i = 0; i < DIR_LIGHT_MAX; ++i) {
        string idx = to_string(i);
        string dir  = "dir_lights[" + idx + "].direction";
        string ambi = "dir_lights[" + idx + "].ambient";
        string diff = "dir_lights[" + idx + "].diffuse";
        string spec = "dir_lights[" + idx + "].specular";
        shader_default3D.dir_lights[i].direction = glGetUniformLocation(shader_default3D.id, dir.c_str());
        shader_default3D.dir_lights[i].ambient = glGetUniformLocation(shader_default3D.id, ambi.c_str());
        shader_default3D.dir_lights[i].diffuse = glGetUniformLocation(shader_default3D.id, diff.c_str());
        shader_default3D.dir_lights[i].specular = glGetUniformLocation(shader_default3D.id, spec.c_str());
    }

    for (int i = 0; i < POINT_LIGHT_MAX; ++i) {
        string idx = to_string(i);
        string pos       = "point_lights[" + idx + "].position";
        string constant  = "point_lights[" + idx + "].constant";
        string linear    = "point_lights[" + idx + "].linear";
        string quadratic = "point_lights[" + idx + "].quadratic";
        string ambi      = "point_lights[" + idx + "].ambient";
        string diff      = "point_lights[" + idx + "].diffuse";
        string spec      = "point_lights[" + idx + "].specular";
        shader_default3D.point_lights[i].position = glGetUniformLocation(shader_default3D.id, pos.c_str());
        shader_default3D.point_lights[i].constant = glGetUniformLocation(shader_default3D.id, constant.c_str());
        shader_default3D.point_lights[i].linear = glGetUniformLocation(shader_default3D.id, linear.c_str());
        shader_default3D.point_lights[i].quadratic = glGetUniformLocation(shader_default3D.id, quadratic.c_str());
        shader_default3D.point_lights[i].ambient = glGetUniformLocation(shader_default3D.id, ambi.c_str());
        shader_default3D.point_lights[i].diffuse = glGetUniformLocation(shader_default3D.id, diff.c_str());
        shader_default3D.point_lights[i].specular = glGetUniformLocation(shader_default3D.id, spec.c_str());
    }

    for (int i = 0; i < SPOT_LIGHT_MAX; ++i) {
        string idx = to_string(i);
        string pos          = "spot_lights[" + idx + "].position";
        string dir          = "spot_lights[" + idx + "].direction";
        string cutoff       = "spot_lights[" + idx + "].cutoff";
        string cutoff_outer = "spot_lights[" + idx + "].cutoff_outer";
        string constant     = "spot_lights[" + idx + "].constant";
        string linear       = "spot_lights[" + idx + "].linear";
        string quadratic    = "spot_lights[" + idx + "].quadratic";
        string ambi         = "spot_lights[" + idx + "].ambient";
        string diff         = "spot_lights[" + idx + "].diffuse";
        string spec         = "spot_lights[" + idx + "].specular";
        shader_default3D.spot_lights[i].position = glGetUniformLocation(shader_default3D.id, pos.c_str());
        shader_default3D.spot_lights[i].direction = glGetUniformLocation(shader_default3D.id, dir.c_str());
        shader_default3D.spot_lights[i].cutoff = glGetUniformLocation(shader_default3D.id, cutoff.c_str());
        shader_default3D.spot_lights[i].cutoff_outer = glGetUniformLocation(shader_default3D.id, cutoff_outer.c_str());
        shader_default3D.spot_lights[i].constant = glGetUniformLocation(shader_default3D.id, constant.c_str());
        shader_default3D.spot_lights[i].linear = glGetUniformLocation(shader_default3D.id, linear.c_str());
        shader_default3D.spot_lights[i].quadratic = glGetUniformLocation(shader_default3D.id, quadratic.c_str());
        shader_default3D.spot_lights[i].ambient = glGetUniformLocation(shader_default3D.id, ambi.c_str());
        shader_default3D.spot_lights[i].diffuse = glGetUniformLocation(shader_default3D.id, diff.c_str());
        shader_default3D.spot_lights[i].specular = glGetUniformLocation(shader_default3D.id, spec.c_str());
    }

    render_clear_dir_lights();
    render_clear_point_lights();
    render_clear_spot_lights();
}

void render_set_camera(Camera& cam)
{
    glUseProgram(shader_default3D.id);

    glUniform3fv(shader_default3D.eye, 1, cam.pos.raw);
    glUniformMatrix4fv(shader_default3D.view, 1, GL_FALSE, (float*)&cam.view);
    glUniformMatrix4fv(shader_default3D.proj, 1, GL_FALSE, (float*)&cam.proj);
}

void render_add_dir_light(Shader& shader, Dir_Light& light)
{
    glUseProgram(shader_default3D.id);

    glUniform3fv(shader.dir_lights[shader.dir_light_idx].direction, 1, light.direction.raw);
    glUniform3fv(shader.dir_lights[shader.dir_light_idx].ambient, 1, light.ambient.raw);
    glUniform3fv(shader.dir_lights[shader.dir_light_idx].diffuse, 1, light.diffuse.raw);
    glUniform3fv(shader.dir_lights[shader.dir_light_idx].specular, 1, light.specular.raw);

    shader.dir_light_idx = (shader.dir_light_idx + 1) % DIR_LIGHT_MAX;
}

void render_add_point_light(Shader& shader, Point_Light& light)
{
    glUseProgram(shader_default3D.id);

    glUniform3fv(shader.point_lights[shader.point_light_idx].position, 1, light.position.raw);
    glUniform1f(shader.point_lights[shader.point_light_idx].constant, light.constant);
    glUniform1f(shader.point_lights[shader.point_light_idx].linear, light.linear);
    glUniform1f(shader.point_lights[shader.point_light_idx].quadratic, light.quadratic);
    glUniform3fv(shader.point_lights[shader.point_light_idx].ambient, 1, light.ambient.raw);
    glUniform3fv(shader.point_lights[shader.point_light_idx].diffuse, 1, light.diffuse.raw);
    glUniform3fv(shader.point_lights[shader.point_light_idx].specular, 1, light.specular.raw);

    shader.point_light_idx = (shader.point_light_idx + 1) % POINT_LIGHT_MAX;
}

void render_add_spot_light(Shader& shader, Spot_Light& light)
{
    glUseProgram(shader_default3D.id);

    glUniform3fv(shader.spot_lights[shader.spot_light_idx].position, 1, light.position.raw);
    glUniform3fv(shader.spot_lights[shader.spot_light_idx].direction, 1, light.direction.raw);
    glUniform1f(shader.spot_lights[shader.spot_light_idx].cutoff, light.cutoff);
    glUniform1f(shader.spot_lights[shader.spot_light_idx].cutoff_outer, light.cutoff_outer);
    glUniform1f(shader.spot_lights[shader.spot_light_idx].constant, light.constant);
    glUniform1f(shader.spot_lights[shader.spot_light_idx].linear, light.linear);
    glUniform1f(shader.spot_lights[shader.spot_light_idx].quadratic, light.quadratic);
    glUniform3fv(shader.spot_lights[shader.spot_light_idx].ambient, 1, light.ambient.raw);
    glUniform3fv(shader.spot_lights[shader.spot_light_idx].diffuse, 1, light.diffuse.raw);
    glUniform3fv(shader.spot_lights[shader.spot_light_idx].specular, 1, light.specular.raw);

    shader.spot_light_idx = (shader.spot_light_idx + 1) % SPOT_LIGHT_MAX;
}

void render(Model& model, vec3s& pos, vec3s& scale, float angle, vec3s& axis, vec4s& tint, bool use_wireframe)
{
    if (use_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glUseProgram(shader_default3D.id);

    mat4s modl = GLMS_MAT4_IDENTITY;
    modl = glms_translate(modl, pos);
    modl = glms_scale(modl, scale);
    modl = glms_rotate(modl, angle, axis);

    glBindTexture(GL_TEXTURE_2D, model.texture);
    glBindVertexArray(model.mesh);

    glUniformMatrix4fv(shader_default3D.modl, 1, GL_FALSE, (float*)&modl);

    glUniform3fv(shader_default3D.material.ambient, 1, model.material.ambient.raw);
    glUniform3fv(shader_default3D.material.diffuse, 1, model.material.diffuse.raw);
    glUniform3fv(shader_default3D.material.specular, 1, model.material.specular.raw);
    glUniform1f(shader_default3D.material.shininess, model.material.shininess);
    glUniform1f(shader_default3D.material.smoothness, model.material.smoothness);

    glUniform4fv(shader_default3D.tint, 1, (float*)&tint);
    glUniform1i(shader_default3D.use_tex, model.texture > 0);

    glDrawElements(GL_TRIANGLES, model.triangles, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (use_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}




























