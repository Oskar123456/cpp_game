#ifndef RENDER_H
#define RENDER_H

#include <util.h>
#include <camera.h>

#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include <cglm/struct.h>

struct Material {
    vec3s ambient, diffuse, specular;
    float shininess, smoothness;
};

struct Model {
    u32 mesh; // vao
    u32 vertices, triangles;
    u32 texture;
    Material material;
};

struct Dir_Light {
    vec3s direction;
    vec3s ambient, diffuse, specular;
};

struct Point_Light {
    vec3s position;
    float constant, linear, quadratic;
    vec3s ambient, diffuse, specular;
};

struct Spot_Light {
    vec3s position, direction;
    float cutoff, cutoff_outer;
    float constant, linear, quadratic;
    vec3s ambient, diffuse, specular;
};

typedef struct Shader Shader;

void render_init();
void render_set_camera(Camera& cam);
void render_clear_dir_lights();
void render_clear_point_lights();
void render_clear_spot_lights();
void render_add_dir_light(Shader& shader, Dir_Light& light);
void render_add_point_light(Shader& shader, Point_Light& light);
void render_add_spot_light(Shader& shader, Spot_Light& light);
void render(Model& model, vec3s& pos, vec3s& scale, float angle, vec3s& axis, vec4s& tint, bool use_wireframe);
Shader& render_get_shader_default();

#endif
