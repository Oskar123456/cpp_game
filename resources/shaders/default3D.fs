#version 330 core

#define DIR_LIGHT_MAX 1
#define POINT_LIGHT_MAX 20
#define SPOT_LIGHT_MAX 20

struct Material {
    vec3 ambient, diffuse, specular;
    float shininess, smoothness;
};

struct Dir_Light {
    vec3 direction;
    vec3 ambient, diffuse, specular;
};

struct Point_Light {
    vec3 position;
    float constant, linear, quadratic;
    vec3 ambient, diffuse, specular;
};

struct Spot_Light {
    vec3 position, direction;
    float cutoff, cutoff_outer;
    float constant, linear, quadratic;
    vec3 ambient, diffuse, specular;
};

in vec2 frag_tex_coord;
in vec3 frag_world_coord;
in vec3 frag_normal;

uniform sampler2D tex;
uniform vec4 tint;
uniform bool use_tex;
uniform Material material;
uniform Dir_Light dir_lights[DIR_LIGHT_MAX];
uniform Point_Light point_lights[POINT_LIGHT_MAX];
uniform Spot_Light spot_lights[SPOT_LIGHT_MAX];
uniform vec3 eye;

out vec4 final_color;

vec3 CalcLight_Dir(Dir_Light light, vec3 view_dir, vec3 normal, vec3 tex_col);
vec3 CalcLight_Spot(Spot_Light light, vec3 view_dir, vec3 normal, vec3 tex_col);
vec3 CalcLight_Point(Point_Light light, vec3 view_dir, vec3 normal, vec3 tex_col);

void main()
{
    vec3 normal = normalize(frag_normal);
    vec3 view_dir = normalize(eye - frag_world_coord);

    vec4 tex_col = texture(tex, frag_tex_coord);
    vec3 color = vec3(0);
    
    for (int i = 0; i < DIR_LIGHT_MAX; ++i)
        color += CalcLight_Dir(dir_lights[i], view_dir, normal, vec3(tex_col));
    for (int i = 0; i < POINT_LIGHT_MAX; ++i)
        color += CalcLight_Point(point_lights[i], view_dir, normal, vec3(tex_col));
    // for (int i = 0; i < SPOT_LIGHT_MAX; ++i)
    //     color += CalcLight_Spot(spot_lights[i], view_dir, normal, vec3(tex_col));
    
    if (!(tint == vec4(1, 1, 1, 1)))
        color = mix(color, vec3(tint), 0.5);
    
    final_color = vec4(color, tex_col.a);
}

vec3 CalcLight_Dir(Dir_Light light, vec3 view_dir, vec3 normal, vec3 tex_col)
{
    vec3 light_dir = normalize(-light.direction);
    vec3 halfway_vec = normalize(view_dir + light_dir);
    float diff = max(dot(normal, light_dir), 0.0);
    
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, halfway_vec), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * tex_col;
    vec3 diffuse = light.diffuse * diff * tex_col;
    vec3 specular = material.smoothness * light.specular * spec * tex_col;
    
    return (max(ambient, 0) + max(diffuse, 0) + max(specular, 0));
}

vec3 CalcLight_Point(Point_Light light, vec3 view_dir, vec3 normal, vec3 tex_col)
{
    vec3 light_dir = normalize(light.position - frag_world_coord);
    vec3 halfway_vec = normalize(view_dir + light_dir);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, halfway_vec), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - frag_world_coord);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * tex_col;
    vec3 diffuse = light.diffuse * diff * tex_col;
    vec3 specular = material.smoothness * light.specular * spec * tex_col * diff;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (max(ambient, 0) + max(diffuse, 0) + max(specular, 0));
}

vec3 CalcLight_Spot(Spot_Light light, vec3 view_dir, vec3 normal, vec3 tex_col)
{
    vec3 light_dir = normalize(light.position - frag_world_coord);
    vec3 halfway_vec = normalize(view_dir + light_dir);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, halfway_vec), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - frag_world_coord);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Spot_Light intensity
    float theta = dot(light_dir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.cutoff_outer;
    float intensity = clamp((theta - light.cutoff_outer) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * tex_col;
    vec3 diffuse = light.diffuse * diff * tex_col;
    vec3 specular = material.smoothness * light.specular * spec * tex_col;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (max(ambient, 0) + max(diffuse, 0) + max(specular, 0));
}
