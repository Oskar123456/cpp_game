#version 330 core

const int lights_max = 10;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float smoothness;
} material;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 direction;
    float attenuation;
} lights[lights_max];

in vec2 tex_coord;
in vec3 world_coord;
in vec3 normal;

uniform sampler2D tex;
uniform vec4 tint;
uniform bool use_tex;
uniform int lights_num;

out vec4 final_color;

void main()
{
    if (!use_tex)
        final_color = tint;
    else if (tint == vec4(1, 1, 1, 1))
        final_color = texture(tex, tex_coord);
    else
        final_color = mix(texture(tex, tex_coord), tint, 0.5);
}
