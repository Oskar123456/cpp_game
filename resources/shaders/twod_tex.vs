#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

uniform mat4 proj;

out vec2 tex_coord;

void main()
{
    gl_Position = proj * vec4(in_pos, 1);
    tex_coord = in_tex_coord;
}
