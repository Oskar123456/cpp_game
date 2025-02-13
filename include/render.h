#ifndef RENDER_H
#define RENDER_H

#include <util.h>
#include <camera.h>

#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include <cglm/struct.h>

struct Material {
    vec3s ambient, diffuse, specular;
    float smoothness;
};

struct Light {
    vec3s ambient, diffuse, specular;
    vec3s position, direction;
    float attenuation;
};

struct Model {
    u32 mesh; // vao
    u32 vertices, triangles;
    u32 texture;
    Material material;
};

void render_init();
void render_add_lights(Light* lights, int n);
void render(Camera& cam, Model& model, vec3s& pos, vec3s& scale, float angle, vec3s& axis, vec4s& tint, bool use_wireframe);

#endif
