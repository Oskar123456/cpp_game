#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;
layout (location = 2) in vec3 in_normal;

uniform mat4 modl;
uniform mat4 view;
uniform mat4 proj;

out vec2 frag_tex_coord;
out vec3 frag_world_coord;
out vec3 frag_normal;

void main()
{
    gl_Position = proj * view * modl * vec4(in_pos, 1);
    frag_tex_coord = in_tex_coord;
    frag_world_coord = vec3(modl * vec4(in_pos, 1));
    frag_normal = mat3(transpose(inverse(modl))) * in_normal;  
}
